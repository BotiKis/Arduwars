#include "AWGameSceneMultiplayer.h"

#include "SpriteAssets.h"
#include "SpriteAnimations.h"
#include "UnitSprites.h"
#include "Tileset.h"

#include "Localization.h"
#include "FlashString.h"
#include "PointMath.h"

AWGameSceneMultiplayer::AWGameSceneMultiplayer(MapID mapID)
{
  // store map id
  _mapID = mapID;
}

void AWGameSceneMultiplayer::willBecomeActive(EngineBoy<GameContext, GameSceneID> & engine)
{
  this->gameSceneID = AWGameState::Playing;

  // reset players
  this->player1.reset();
  this->player2.reset();

  this->currentplayer = &this->player1;
  this->roundState = AWGameCoreGameState::Default;

  // load map
  this->loadMap(_mapID);
}

void AWGameSceneMultiplayer::didBecomeInActive(EngineBoy<GameContext, GameSceneID> & engine)
{

}

void AWGameSceneMultiplayer::update(EngineBoy<GameContext, GameSceneID> & engine)
{
  switch (this->roundState) {
    case AWGameCoreGameState::Default: break;
    case AWGameCoreGameState::GameMenu: break;
    case AWGameCoreGameState::ShopMenu: break;
    case AWGameCoreGameState::UnitAttack: break;
    case AWGameCoreGameState::UnitMenu: break;
    default: break;
  }
}

void AWGameSceneMultiplayer::render(EngineBoy<GameContext, GameSceneID> & engine)
{
  // get game context
  auto gameContext = engine.getContext();

  // get Tinyfont
  auto tinyfont = gameContext.tinyfont;
  tinyfont.setTextColor(WHITE);
  tinyfont.setCursor(0,0);
  tinyfont.print("MULTIPLAYER");

  switch (this->roundState) {
    case AWGameCoreGameState::GameMenu: break;
    case AWGameCoreGameState::ShopMenu: break;
    case AWGameCoreGameState::UnitAttack: break;
    case AWGameCoreGameState::UnitMenu: break;
    case AWGameCoreGameState::Default:
    default: break;
  }

  //this->drawMapAtPosition({0,0}, engine);
}

void AWGameSceneMultiplayer::loadMap(MapID mapID)
{
  RawMapData *mapData = nullptr;

  // get actual map data
  switch (mapID)
  {
    case MapID::Map1: mapData = mapData_1; break;
    case MapID::Map2: mapData = mapData_2; break;
    case MapID::Map3: mapData = mapData_3; break;
    default: break;
  }

  // handle map meta info
  // get size
  this->mapSize.x  = pgm_read_byte(mapData+MAPDATAOFFSET_Size);
  this->mapSize.y  = pgm_read_byte(mapData+MAPDATAOFFSET_Size+1);

  // p1 meta Data
  Point player1StartCityCoords;
  player1StartCityCoords.x = pgm_read_byte(mapData+MAPDATAOFFSET_Player1City);
  player1StartCityCoords.y = pgm_read_byte(mapData+MAPDATAOFFSET_Player1City+1);

  Point player1StartWorkshopCoords;
  player1StartWorkshopCoords.x = pgm_read_byte(mapData+MAPDATAOFFSET_Player1Workshop);
  player1StartWorkshopCoords.y = pgm_read_byte(mapData+MAPDATAOFFSET_Player1Workshop+1);

  // p2 meta Data
  Point player2StartCityCoords;
  player2StartCityCoords.x = pgm_read_byte(mapData+MAPDATAOFFSET_Player2City);
  player2StartCityCoords.y = pgm_read_byte(mapData+MAPDATAOFFSET_Player2City+1);

  Point player2StartWorkshopCoords;
  player2StartWorkshopCoords.x = pgm_read_byte(mapData+MAPDATAOFFSET_Player2Workshop);
  player2StartWorkshopCoords.y = pgm_read_byte(mapData+MAPDATAOFFSET_Player2Workshop+1);

  // Load map data
  // Calculate map lenght
  uint16_t mapLenght = mapSize.x*mapSize.y;

  // // The index in the loop will be stored here
  // Point currentIndex;
  //
  // // Populate array from map data
  // for (uint16_t i = 0; i < mapLenght; i++) {
  //
  //   // get Tile Data
  //   mapTileData[i].tileID = pgm_read_byte(mapData+MAPDATAOFFSET_Main+i);
  //   mapTileData[i].buildingIsOccupied = 0;
  //   mapTileData[i].buildingBelongsTo = 0;
  //   mapTileData[i].hasUnit = 0;
  //   mapTileData[i].unitBelongsTo = 0;
  //
  //   MapTileType tileType = static_cast<MapTileType>(mapTileData[i].tileID);
  //
  //   // calc index
  //   currentIndex.x = i % mapSize.x;
  //   currentIndex.y = i / mapSize.x;
  //
  //   // Check if it's a building
  //   if (mapTileIndexIsBuilding(tileType)) {
  //     GameBuilding building = GameBuilding();
  //
  //     // set upd building
  //     building.mapPosX = currentIndex.x;
  //     building.mapPosY = currentIndex.y;
  //     building.isOccupied = 0;
  //     building.belongsToPlayer = 0;
  //     building.buildingType = mapTileData[i].tileID;
  //
  //     // Check for city
  //     if (tileType == MapTileType::City) {
  //       // check for ownership
  //       if (currentIndex == player1StartCityCoords) {
  //         building.isOccupied = 1;
  //         building.belongsToPlayer = MapTile::BelongsToPlayer1;
  //       }
  //       else if (currentIndex == player2StartCityCoords) {
  //         building.isOccupied = 1;
  //         building.belongsToPlayer = MapTile::BelongsToPlayer2;
  //       }
  //     }
  //
  //     // Check for Factory
  //     if (tileType == MapTileType::Factory) {
  //       // check for ownership
  //       if (currentIndex == player1StartWorkshopCoords) {
  //         building.isOccupied = 1;
  //         building.belongsToPlayer = MapTile::BelongsToPlayer1;
  //       }
  //       else if (currentIndex == player2StartWorkshopCoords) {
  //         building.isOccupied = 1;
  //         building.belongsToPlayer = MapTile::BelongsToPlayer2;
  //       }
  //     }
  //
  //     // Check for Headquarters
  //     if (tileType == MapTileType::P1HQ){
  //       building.isOccupied = 1;
  //       building.belongsToPlayer = MapTile::BelongsToPlayer1;
  //       player1.cursorIndex = currentIndex;
  //     }
  //     else if (tileType == MapTileType::P2HQ){
  //       building.isOccupied = 1;
  //       building.belongsToPlayer = MapTile::BelongsToPlayer2;
  //       player2.cursorIndex = currentIndex;
  //     }
  //
  //     // add building to our global buildings
  //     gameBuildings.add(building);
  //   }
  // }
}

void AWGameSceneMultiplayer::makeScreenTransition(void)
{
  uint8_t *dbuff = Arduboy2::sBuffer;
  const uint8_t screenWidth = Arduboy2::width();

  for (uint8_t i = 0; i < screenWidth; i++) {
      for (uint8_t x = 0; x < screenWidth; x++) {
        for (uint8_t y = 0; y < 8; y++) {
          if (y%2==0) {
            if (x == screenWidth-1)
              dbuff[x+y*screenWidth] = 0;
            else
              dbuff[x+y*screenWidth] = dbuff[x+y*screenWidth+1];
          }
          else{
            uint8_t helperX = screenWidth-1-x;
            if (helperX == 0)
              dbuff[helperX+y*screenWidth] = 0;
            else
              dbuff[helperX+y*screenWidth] = dbuff[helperX+y*screenWidth-1];
          }
        }
      }

      //arduboy.display();
      #warning "figure out how to call"
      delay(1);
  }
}

void AWGameSceneMultiplayer::showDialog(char_P *titleText)
{

  const uint8_t screenWidth = Arduboy2::width();

  // frame for the dialog
  Rect frame;

  frame.width = strlen_P(titleText)*5+8;
  frame.height = 16;
  frame.x = (screenWidth - frame.width)/2;
  frame.y = 24;

  Point textPos;
  textPos.x = (screenWidth - strlen_P(titleText)*5)/2;
  textPos.y = frame.y+6;


  #warning "figure out how to call"

  // // dialog loop
  // while (true) {
  //
  //   // wait for next frame
  //   if(!arduboy.nextFrame()) continue;
  //
  //   // Get input
  //   arduboy.pollButtons();
  //
  //   // Exit on B
  //   if (arduboy.justPressed(B_BUTTON)){
  //     return;
  //   }
  //
  //   // Drawing
  //   // Infobox
  //   arduboy.fillRoundRect(frame.x, frame.y, frame.width, frame.height, 5, BLACK);
  //   arduboy.fillRoundRect(frame.x + 1, frame.y + 1, frame.width-2, frame.height-3, 5, WHITE);
  //
  //   // OK Button
  //   tinyfont.setCursor(textPos.x, textPos.y);
  //   tinyfont.print(AsFlashString(titleText));
  //
  //   arduboy.display();
  // }
}

void AWGameSceneMultiplayer::drawMapAtPosition(Point aPosition, EngineBoy<GameContext, GameSceneID> & engine)
{
  // helpers
  const uint8_t screenWidth = Arduboy2::width();
  const uint8_t screenHeight = Arduboy2::height();

  // get game context
  auto gameContext = engine.getContext();

  // In this variable we store the tile
  Point drawPos;

  // draw the map
  for (int8_t y = 0; y < mapSize.y; y++) {
    for (int8_t x = mapSize.x-1; x >= 0; x--) { // draw the map from right to left helps with the markers

      drawPos.x = aPosition.x+x*TILE_SIZE;
      drawPos.y = aPosition.y+y*TILE_SIZE;

      // ignore if out of bounds
      if (drawPos.x <= -TILE_SIZE || drawPos.x >= screenWidth || drawPos.y <= -TILE_SIZE || drawPos.y >= (screenHeight+TILE_SIZE)) continue;

      // get the tile
      const MapTile tile = this->mapTileData.getItem(x, y);
      const MapTileType tileType = static_cast<MapTileType>(tile.tileID);

      // draw maptile
      gameContext.sprites.drawSelfMasked(drawPos.x, drawPos.y, worldSprite, tile.tileID);

      // check if tile below is the HQ
      if (y < mapSize.y-1) {
        const MapTile tileBelow = this->mapTileData.getItem(x, y+1);
        const MapTileType tileTypeOfTileBelow = static_cast<MapTileType>(tileBelow.tileID);

        // If it's a headquarter draw upper half at the position above
        if (tileTypeOfTileBelow == MapTileType::P1HQ || tileTypeOfTileBelow == MapTileType::P2HQ) {
          gameContext.sprites.drawOverwrite(drawPos.x, drawPos.y, worldSprite, (tileTypeOfTileBelow == MapTileType::P1HQ)?32:33);
        }
      }

      // draw fog
      if (tile.showsFog && !tile.showSelection) {
          gameContext.sprites.drawErase(drawPos.x, drawPos.y, mapFOG_16x16, 0);
      }

      // Draw marker for buildings
      // we need to identify visually to whom the buiding belongs.
      // Since we have no colors to indicate this, we draw small markers at
      // the top right corner of a building.
      else if (mapTileIndexIsBuilding(tileType) && tileType != MapTileType::P1HQ && tileType != MapTileType::P2HQ) {
        if (tile.buildingIsOccupied){
          if(tile.buildingBelongsTo == MapTile::BelongsToPlayer){
            gameContext.sprites.drawPlusMask(drawPos.x+10, drawPos.y-2, mapMarkers_plus_mask, 0);
          }
          else if(tile.buildingBelongsTo == MapTile::BelongsToEnemy){
            gameContext.sprites.drawPlusMask(drawPos.x+10, drawPos.y-2, mapMarkers_plus_mask, 1);
          }
        }
      }

      // draw unit selection
      if (tile.showSelection) {
        gameContext.sprites.drawErase(drawPos.x, drawPos.y, selectionAnimation, (engine.arduboy.frameCount/10)%4);
      }

      // Draw Unit
      if(tile.hasUnit && !tile.showsFog){
        // unitSprite
        const unsigned char *unitSprite = nullptr;

        // get correct sprite
        if(tile.unitBelongsTo == MapTile::BelongsToPlayer1)
          unitSprite = unitsA_plus_mask;
        else if(tile.unitBelongsTo == MapTile::BelongsToPlayer2)
          unitSprite = unitsB_plus_mask;

        // Check if unit is active
        if (tile.unitIsActive == GameUnit::UnitStateActive) {
          // Draw sprite
          // The *2 is because every unit in the spritesheet has two frames
          // The +(arduboy.frameCount/10)%2 part switches between these two frames
          // based on the current framecount.
          gameContext.sprites.drawPlusMask(drawPos.x, drawPos.y, unitSprite, tile.unitSpriteID*2+(engine.arduboy.frameCount/10)%2);
        }
        else{
          // Don't animate if not active and draw a badge to top right.
          // The *2 is because every unit in the spritesheet has two frames
          gameContext.sprites.drawPlusMask(drawPos.x, drawPos.y, unitSprite, tile.unitSpriteID*2);
          gameContext.sprites.drawPlusMask(drawPos.x+10, drawPos.y-2, mapMarkers_plus_mask, 2);
        }

      }

    }
  }
}

void AWGameSceneMultiplayer::setActivePlayer(AWPlayer &aPlayer)
{
  // set current player
  this->currentplayer = &aPlayer;

  // activate all units
  for (uint8_t i = 0; i < aPlayer.units.getCount(); i++) {
    aPlayer.units[i].activated = GameUnit::UnitStateActive;
  }

  // Calculate Income for every City
  for (uint8_t i = 0; i < gameBuildings.getCount(); i++) {
    if(gameBuildings[i].buildingType == static_cast<uint8_t>(MapTileType::City) &&
    gameBuildings[i].isOccupied &&
    gameBuildings[i].belongsToPlayer == ((*this->currentplayer == this->player1)?MapTile::BelongsToPlayer1:MapTile::BelongsToPlayer2))
    {
      aPlayer.money += AWPlayer::BaseIncome;
    }
  }

  // update player map
  this->updateMapForPlayer(aPlayer);
}

void AWGameSceneMultiplayer::updateMapForPlayer(AWPlayer &aPlayer)
{
  // fill map with fog
  this->clearMap(true);

  // add the enemies units
  AWPlayer &enemyPlayer = (aPlayer == this->player1)?this->player2:this->player1;
  for (uint8_t i = 0; i < enemyPlayer.units.getCount(); i++) {
    GameUnit unit = enemyPlayer.units[i];

    // get the corresponding map tile
    MapTile tile = this->mapTileData.getItem(unit.mapPosX, unit.mapPosY);
    tile.hasUnit = 1;
    tile.unitBelongsTo = (enemyPlayer == this->player1)?MapTile::BelongsToPlayer1:MapTile::BelongsToPlayer2;
    tile.unitSpriteID = unit.unitType;
    tile.unitIsActive = GameUnit::UnitStateActive;
  }

  // udpate the player units
  for (uint8_t i = 0; i < aPlayer.units.getCount(); i++) {
    GameUnit unit = aPlayer.units[i];

    // get the corresponding map tile
    MapTile tile =  this->mapTileData.getItem(unit.mapPosX, unit.mapPosY);
    tile.hasUnit = 1;
    tile.unitBelongsTo = (aPlayer == this->player1)?MapTile::BelongsToPlayer1:MapTile::BelongsToPlayer2;
    tile.unitSpriteID = unit.unitType;
    tile.unitIsActive = unit.activated;

    // undo fog for units
    // Units can see same far as they can move.
    // get the traits of the unit
    UnitType unitType = static_cast<UnitType>(unit.unitType);
    UnitTraits traits = UnitTraits::traitsForUnitType(unitType);
    uint8_t sightRadius = traits.moveDistance;

    // add sight to unit if its on a mountain
    // Only infantry units can be on hills and mountains and they get a sight boost.
    if (tile.tileID == static_cast<uint8_t>(MapTileType::Mountain))
      sightRadius +=1;

    // remove the fog
    // The fog for a unit gets removed accordingly to it's sight.
    // Obstacles will conceal the things behind it so a unit can't
    // see through the enviroment.
    this->removeFogAtPositionRadiusAndPlayer({unit.mapPosX, unit.mapPosY}, sightRadius, aPlayer, false);
  }

  // udpate the  buildings
  for (uint8_t i = 0; i < gameBuildings.getCount(); i++) {
    GameBuilding building = gameBuildings[i];
    uint8_t thisPlayer = (aPlayer == this->player1)?MapTile::BelongsToPlayer1:MapTile::BelongsToPlayer2;

    // get the corresponding map tile
    MapTile tile = this->mapTileData.getItem(building.mapPosX, building.mapPosY);

    // check if building belongs to current player
    if (building.isOccupied) {
      tile.buildingIsOccupied = 1;
      tile.buildingBelongsTo = (building.belongsToPlayer == thisPlayer)?MapTile::BelongsToPlayer:MapTile::BelongsToEnemy;
    }
    else{
      // not occupied
      tile.buildingIsOccupied = 0;
      tile.buildingBelongsTo = 0;
    }

    // check if building belongs to player, because now we remove the fog of war calculations
    if (!(building.isOccupied && building.belongsToPlayer == thisPlayer)) continue;

    // remove fog of war
    this->removeFogAtPositionRadiusAndPlayer({building.mapPosX,building.mapPosY}, GameBuilding::buildingViewDistance, aPlayer, true);
  }
}

void AWGameSceneMultiplayer::clearMap(bool withFog){
  // go trough the whole map
  for (int8_t y = 0; y < mapSize.y; y++) {
    for (int8_t x = 0; x < mapSize.x; x++) {

        // get the tile
        MapTile tile = this->mapTileData.getItem(x, y);

        // turn fog on
        tile.showsFog = withFog?1:0;

        // disable selection
        tile.showSelection = 0;

        // remove unit info
        tile.hasUnit = 0;
        tile.unitBelongsTo = 0;
        tile.unitSpriteID = 0;
    }
  }
}

void AWGameSceneMultiplayer::removeFogAtPositionRadiusAndPlayer(Point origin, uint8_t radius, AWPlayer &aPlayer, bool seeThrough)
{
    // For the raycast to work we need to calculate every point which should
    // be tested around the perimeter of the unit. For that we use Bresenham's
    // circle algorithm to get every point on the perimeter and do a raycast to that point.
    int8_t r = radius*2; // double the distance to avoid glitches
    int8_t x = -r;
    int8_t y = 0;
    int8_t err = 2-2*r;
    do {

      // do a raycast in each quadrant of the circle.
      castRayTo(origin, seeThrough, aPlayer, origin.x-x, origin.y+y); // 1st quadrant
      castRayTo(origin, seeThrough, aPlayer, origin.x-y, origin.y-x); // 2nd quadrant
      castRayTo(origin, seeThrough, aPlayer, origin.x+x, origin.y-y); // 3rd quadrant
      castRayTo(origin, seeThrough, aPlayer, origin.x+y, origin.y+x); // 4th quadrant

      // continue with bresenham
      r = err;
      if (r <= y) err += ++y*2+1;
      if (r > x || err > y) err += ++x*2+1;
    } while (x < 0);
}

void AWGameSceneMultiplayer::castRayTo(Point origin, bool seeThrough, AWPlayer &aPlayer, int8_t xEnd, int8_t yEnd)
{

  // We are doing here a so called Raycast. It's called this way because
  // it mathematecally shots a "ray" from the origin to the destination Like
  // a light ray. If it hits an obstacle it stops so you can't see through.
  // it also stops if it reaches the sight radius.
  //
  // The algorithm which we use to cast the ray is called
  // Bresenham's algorithm and is often used in computer graphics to draw
  // lines and circles. Even the Arduboy library uses it for this purpose.

  int8_t x0 = origin.x;
  int8_t y0 = origin.y;
  int8_t dx =  abs(xEnd-x0), sx = x0<xEnd ? 1 : -1;
  int8_t dy = -abs(yEnd-y0), sy = y0<yEnd ? 1 : -1;
  int8_t err = dx+dy, e2;
  int8_t forestLimit = 2; // Unit can see through 2 forests

  // half the distance, because the endppoint which comes from the circle-
  // algorithm is in a doubled distance to avoid glitches.
  int8_t r = (dx-dy)/2; // There might be rounding issues but for simplicity we don't care.

  while(true){

    // calc current disctance
    uint8_t currentDistance = abs(origin.x-x0) + abs(origin.y-y0);

    // check for bounds and remove fog
    if (x0 >= 0 && y0 >= 0 && x0 < mapSize.x && y0 < mapSize.y && currentDistance <= r) {

      // get the corresponding map tile
      MapTile tile = this->mapTileData.getItem(x0, y0);

      // update tile data
      if (forestLimit >= 0 || seeThrough)
        tile.showsFog = 0;

      // ignore the origin
      if (currentDistance != 0 && !seeThrough){

        // check if there is an Obstacle
        if (mapTileIsOpaque(static_cast<MapTileType>(this->mapTileData.getItem(x0, y0).tileID)))
            break;

        // check if there is an enemy unit because we can't see through it
        if (tile.hasUnit && tile.unitBelongsTo == ((aPlayer == player1) ? MapTile::BelongsToPlayer2 : MapTile::BelongsToPlayer1)){
          break;

        // check for forest
        if (static_cast<MapTileType>(tile.tileID) == MapTileType::Forest)
          forestLimit--;
        }
      }
    }

    // check for end
    if ( x0==xEnd && y0==yEnd) break;

    // update algorithm
    e2 = 2*err;
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}
