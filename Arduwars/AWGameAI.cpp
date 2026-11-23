#include "AWGame.h"
#include "PointMath.h"

// ====================================================
// Enemy AI
//
// This file holds only the AI's thinking. It belongs to the AWGame class,
// so it can use the map, the players and the movement tools of the game.
// The walk itself runs in doRoundOfPlayer, the same as for the player.
//
// The AI is simple and greedy. Every unit looks at all tiles it can reach
// this turn and picks the one with the best score: attacking for the most
// damage, capturing, or getting closer to its goal. Then the AI buys units
// at every free factory, airport and shipyard it owns.
//
// The AI plays fair: it only knows about the enemy units its own units can
// see. When it sees none, it marches on the enemy HQ.

uint8_t AWGame::nextRandom(){
  // Multiply by 5, add 1. This visits all 256 numbers in a fixed shuffled
  // order before it repeats. The game start sets where we begin.
  randomState = randomState*5 + 1;
  return randomState;
}

Point AWGame::aiPlanMove(GameUnit *unit, AWPlayer &ai, Point &target){
  AWPlayer &enemy = (ai == player1) ? player2 : player1;
  UnitType unitType = static_cast<UnitType>(unit->unitType);
  bool indirect = UnitTraits::traitsForUnitType(unitType).attackRange > 1;
  Point start = {unit->mapPosX, unit->mapPosY};

  // While the AI thinks, the map holds the AI's own fog.
  updateMapForPlayer(ai);

  // A second map of numbers, in the half of the screen buffer behind the
  // movement range (see floodMovement for why that is safe).
  // A number is the movement left after travelling there: higher is closer.
  uint8_t *field = arduboy.getBuffer() + MaxMapWidth*MaxMapHeight;
  constexpr int8_t FarAway = 127;

  // Goal: the closest enemy unit we can see, the enemy HQ, or for infantry
  // also a building it can capture. Closest by a real route over terrain
  // this unit can cross, so land units don't head for targets across water.
  floodFrom(start, unitType, FarAway, field);
  Point goal = start;
  uint8_t goalCloseness = 0;
  uint8_t candidates = enemy.units.getCount() + gameBuildings.getCount();
  for (uint8_t i = 0; i < candidates; i++) {
    Point position;
    if (i < enemy.units.getCount()) {
      position = {enemy.units[i].mapPosX, enemy.units[i].mapPosY};

      // units hidden in our fog don't count
      if (tileAt(position.x, position.y).showsFog) continue;
    }
    else {
      GameBuilding &building = gameBuildings[i - enemy.units.getCount()];
      position = {building.mapPosX, building.mapPosY};
      MapTileType type = static_cast<MapTileType>(building.buildingType);
      bool enemyHQ = (type == MapTileType::P1HQ || type == MapTileType::P2HQ) && building.belongsToPlayer != playerIndex(ai);
      if (!enemyHQ && !canUnitCaptureAtPosition(unit, position, ai)) continue;
    }

    // a goal counts as reachable if we can get onto it or next to it
    for (uint8_t direction = 0; direction < 5; direction++) {
      int8_t x = position.x + (direction == 0) - (direction == 2);
      int8_t y = position.y + (direction == 3) - (direction == 1);
      if (x < 0 || x >= mapSize.x || y < 0 || y >= mapSize.y) continue;
      if (field[y*mapSize.x+x] > goalCloseness) {
        goalCloseness = field[y*mapSize.x+x];
        goal = position;
      }
    }
  }

  // Route distance from every tile to the goal
  floodFrom(goal, unitType, FarAway, field);

  // Score every reachable free tile
  uint8_t *remaining = floodMovement(unit);
  Point best = start;
  target = {-1, -1};
  int16_t bestScore = -32000;

  for (uint16_t i = 0; i < mapSize.x*mapSize.y; i++) {
    if (remaining[i] == 0) continue;
    Point tile = {(int16_t)(i % mapSize.x), (int16_t)(i / mapSize.x)};
    bool isStart = (tile == start);
    // tiles with a unit we can see are taken (a hidden enemy's tile looks free)
    if (!isStart && mapTileData[i].hasUnit && !mapTileData[i].showsFog) continue;

    // get closer to the goal along a real route, prefer cover
    int16_t score = 4*field[i] + mapTileDefenseBonus(tileTypeAtIndex(i));

    // capture, the enemy HQ most of all
    if (canUnitCaptureAtPosition(unit, tile, ai)) {
      MapTileType type = tileTypeAtIndex(i);
      score += (type == MapTileType::P1HQ || type == MapTileType::P2HQ) ? 1000 : 200;
    }

    // attack (indirect units only without moving)
    Point tileTarget = {-1, -1};
    if (!indirect || isStart) {
      uint8_t damage = scanTargets(unit, tile, ai, tileTarget, false);
      if (damage > 0) score += 100 + 10*damage;
    }

    // A random 0-3 on top only decides between tiles that tie.
    score = 4*score + (nextRandom() >> 6);

    if (score > bestScore) {
      bestScore = score;
      best = tile;
      target = tileTarget;
    }
  }

  // The path follows the AI's own fog: it may run into a hidden enemy.
  buildPathTo(unit, best);

  // back to player 1's view for the walk
  updateMapForPlayer(player1);
  return best;
}

void AWGame::aiAct(GameUnit *unit, Point target, AWPlayer &ai, bool ambushed){
  // the unit is done for this turn
  unit->activated = GameUnit::UnitStateDisabled;

  // Ambushed: the plan is off. Attack a unit next to it (direct units
  // only, it has moved), else just wait. No capture.
  // The tiles next to a unit are never in its fog, so player 1's view
  // shows the same neighbours as the AI's own.
  if (ambushed) {
    target = {-1, -1};
    if (UnitTraits::traitsForUnitType(static_cast<UnitType>(unit->unitType)).attackRange == 1)
      scanTargets(unit, {unit->mapPosX, unit->mapPosY}, ai, target, false);
    if (target.x < 0) return;
  }

  // Note: the unit may be destroyed by the counterattack,
  // so it must not be used after this call.
  if (target.x >= 0)
    attackUnitAtPosition(unit, target, ai);
  else
    captureBuildingWithUnit(unit, ai);
}

void AWGame::aiBuyUnits(AWPlayer &ai){

  // Count infantry: keep at least 3 around for capturing
  uint8_t infantry = 0;
  for (uint8_t k = 0; k < ai.units.getCount(); k++)
    if (GameUnit::isInfantry(static_cast<UnitType>(ai.units[k].unitType))) infantry++;

  // Buy at every free shop we own
  for (uint8_t k = 0; k < gameBuildings.getCount(); k++) {
    GameBuilding &building = gameBuildings[k];
    Point position = {building.mapPosX, building.mapPosY};
    if (!building.isOccupied || building.belongsToPlayer != playerIndex(ai)) continue;
    if (player1.getUnitForMapCoordinates(position) || player2.getUnitForMapCoordinates(position)) continue;
    if (ai.units.isFull()) break;

    const UnitType *buyableUnits = nullptr;
    uint8_t numberOfUnits = shopListForBuilding(static_cast<MapTileType>(building.buildingType), buyableUnits);

    // the cheapest unit if we need infantry, else the most expensive one that can fight
    UnitType choice = UnitType::None;
    uint8_t choicePrice = 0;
    for (uint8_t u = 0; u < numberOfUnits; u++) {
      UnitType candidate = static_cast<UnitType>(pgm_read_byte(buyableUnits+u));
      uint8_t price = GameUnit::costsOfUnit(candidate);
      if (price > ai.money || UnitTraits::traitsForUnitType(candidate).attackPower == 0) continue;
      if (choice == UnitType::None || (infantry >= 3 && price > choicePrice)) {
        choice = candidate;
        choicePrice = price;
      }
    }
    if (choice == UnitType::None) continue;

    addNewUnit(ai, choice, position);
    ai.money -= choicePrice;
    if (GameUnit::isInfantry(choice)) infantry++;
  }
  updateMapForPlayer(player1);
}
