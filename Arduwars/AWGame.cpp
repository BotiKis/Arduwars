#include "AWGame.h"

#include "Tileset.h"
#include "SpriteAnimations.h"
#include "SpriteAssets.h"
#include "MapData.h"
#include "ShopData.h"

#include <UtilityFunctions.h>
#include <Utility.h>
#include <FlashString.h>
#include "PointMath.h"
#include "UnitSprites.h"

// This method starts the actual game and is called in the Arduwars.ino file.
void AWGame::run(void){

  // First we need to initialize our Arduboy instance.
  // boot() instead of begin() skips the ARDUBOY boot logo to save flash.
  // flashlight() keeps the safe mode: hold UP while turning on to be able
  // to upload a new sketch even if the current one misbehaves.
  arduboy.boot();
  arduboy.flashlight();
  arduboy.setFrameRate(60);
  arduboy.audio.begin();

  // Set up text
  tinyfont.setTextColor(BLACK);

  // Game loop
  // The gameloop works like the loop() in the Arduwars.ino file.
  // It repeats itself indefinitely and will only stop if you turn off you Arduboy.
  while(true){

      // Inside the loop we take a look at the gamestate and depending on it
      // we desiced what to do next.
      // At the start the gamestate will be AWGameState::showMenu.

      switch (gameState) {
        case AWGameState::playSinglePlayer:{
          // start game
          startNewGame(AWGameState::playSinglePlayer);

          // If played, return to main menu
          this->gameState = AWGameState::showMainMenu;
          break;
        }
        case AWGameState::playMultiPlayer:{
          // start game
          startNewGame(AWGameState::playMultiPlayer);

          // If played, return to main menu
          this->gameState = AWGameState::showMainMenu;
          break;
        }
        case AWGameState::showMainMenu:
        default:
          // When showMenu is the current state, we will call the showMenu() method.
          // Take a look at the method now!
          this->gameState = showMainMenu();
      }
  }
}


// Title screen with a vertical menu below the logo.
// With a subtitle the menu can be left with A (returns -1) and shows a hint.
int8_t AWGame::showTitleMenu(char_P * const items[], uint8_t count, char_P *subtitle, int8_t cursorIdx){
  // cursorIdx is the index of the cursor, so we know what the player has selected.
  constexpr uint8_t yOffset = 44;

  // Again a Game loop
  while(true){

    // wait for next frame since we don't want to do things more than that.
    // There may be isntances where we would do stuff between frames (e.g. smmulations, calculations, etc.)
    // but for this project we stick with this.
    if(!arduboy.nextFrame()) continue;

    // Now we handle the button input of the arduboy
    arduboy.pollButtons();

    // Here we limit and wrap the cursor so it cannot be larger than count-1 nor smaller then 0.
    if (arduboy.justPressed(UP_BUTTON)) cursorIdx--;
    if (arduboy.justPressed(DOWN_BUTTON)) cursorIdx++;
    if (cursorIdx < 0) cursorIdx = count-1;
    if (cursorIdx >= count) cursorIdx = 0;

    // React to users choice
    if (arduboy.justPressed(B_BUTTON)) return cursorIdx;

    // When the A Button is pressed return nothing.
    if (subtitle != nullptr && arduboy.justPressed(A_BUTTON)) return -1;

    // Now we start drawing the menu to the screen

    // This clears the dispaly and makes everything black.
    arduboy.clear();

    // draw title logo
    sprites.drawSelfMasked(7, 0, arduwarsLogo, 0);

    // Here we draw the menu text
    tinyfont.setTextColor(WHITE);

    if (subtitle != nullptr) {
      tinyfont.setCursor((arduboy.width() - strlen_P(subtitle)*5)/2, 20);
      tinyfont.print(AsFlashString(subtitle));
      tinyfont.setCursor(2, yOffset-8);
      tinyfont.print(AsFlashString(LOCA_BackWithA));
    }

    for (uint8_t i = 0; i < count; i++) {
      tinyfont.setCursor(10, yOffset + 8*i);
      tinyfont.print(AsFlashString(items[i]));
    }

    tinyfont.setCursor(2, yOffset + 8*cursorIdx);
    tinyfont.print(AsFlashString(LOCA_Cursor));

    tinyfont.setTextColor(BLACK);

    // this one draws everything we have drawn to the actual screen.
    arduboy.display();

    // Now this loop wraps around and keeps repeating until the user selects
    // an menu item.
  }
}

// This method displays the Main menu to the player.
AWGameState AWGame::showMainMenu(){

  // In this variable we will store the index of the cursor so
  // we know what the player has selected.
  int8_t cursorIdx = 0;
  while (true) {
    char_P* items[3] = {LOCA_SinglePlayer, LOCA_MultiPlayer, arduboy.audio.enabled() ? LOCA_Sound_on : LOCA_Sound_off};
    cursorIdx = showTitleMenu(items, 3, nullptr, cursorIdx);

    if (cursorIdx == 0) return AWGameState::playSinglePlayer;
    if (cursorIdx == 1) return AWGameState::playMultiPlayer;

    // toggle sound and stay in the menu
    arduboy.audio.toggle();
  }
}

// This method displays the map selection menu to the player.
// It returns the selected map in progmem, nullptr if cancelled.
const unsigned char * AWGame::showMapSelection(AWGameState aState){
  char_P* items[3] = {LOCA_mapSmall, LOCA_mapMedium, LOCA_mapBig};

  // select map to users choice
  switch (showTitleMenu(items, 3, (aState == AWGameState::playSinglePlayer) ? LOCA_SinglePlayer : LOCA_MultiPlayer, 0)) {
    case 0: return mapData_3;
    case 1: return mapData_2;
    case 2: return mapData_1;
    default: return nullptr;
  }
}

void AWGame::startNewGame(AWGameState mode){
  // reset day count and result
  daysPlayed = 0;
  winner = NoWinner;

  // reset players
  player1.reset();
  player2.reset();

  // shop map selection
  const unsigned char *mapData = showMapSelection(mode);

  // return if no map has been selected
  if (mapData == nullptr) return;

  // The time the player took in the menus starts the AI's randomness.
  randomState = arduboy.frameCount;

  // load map
  loadMap(mapData);

  // In single player the AI plays player 2.
  runGame(mode == AWGameState::playSinglePlayer);
}

// Adds health to a unit, up to its full health.
// Kept out of line on purpose: two callers, and inlining costs more flash.
static void __attribute__((noinline)) healUnit(GameUnit &unit, uint8_t amount){
  uint8_t health = unit.healthPoints + amount;
  if (health > GameUnit::UnitDefaultHealth) health = GameUnit::UnitDefaultHealth;
  unit.healthPoints = health;
}

void AWGame::runGame(bool againstAI){

  AWPlayer *currentPlayer = &player1;

  // the first turn wipes in from the map selection
  bool firstTurn = true;

  // Game Loop
  while (true) {

    // the AI plays player 2 in single player
    bool aiTurn = againstAI && *currentPlayer == player2;

    // Against the AI the player's turn starts right on the map, the view
    // stays the same. Otherwise the old view wipes out and the turn text
    // shows on black, so a second player can't peek.
    bool startOnMap = againstAI && !aiTurn;
    bool wipe = !startOnMap || firstTurn;
    firstTurn = false;

    if (wipe) makeScreenTransition(false);

    // show dialog for player
    if (!startOnMap)
      showDialog(aiTurn ? LOCA_enemyTurn : (*currentPlayer == player1) ? LOCA_player1 : LOCA_player2);

    // activate all units
    for (uint8_t i = 0; i < currentPlayer->units.getCount(); i++) {
      currentPlayer->units[i].activated = GameUnit::UnitStateActive;
    }

    // capture progress is lost when the capturing unit left or was destroyed
    resetAbandonedCaptures();

    // Own buildings: every city pays income, and a unit on a building of the
    // right kind heals 2. Planes heal at an airport, ships at a shipyard,
    // land units at every other building.
    for (uint8_t i = 0; i < gameBuildings.getCount(); i++) {
      GameBuilding &building = gameBuildings[i];
      if (!building.isOccupied || building.belongsToPlayer != playerIndex(*currentPlayer)) continue;

      MapTileType type = static_cast<MapTileType>(building.buildingType);
      if (type == MapTileType::City) {
        // money stops at 255, more would wrap around to 0
        currentPlayer->money = min(currentPlayer->money + AWPlayer::BaseIncome, 255);
      }

      Point position = {building.mapPosX, building.mapPosY};
      GameUnit *unit = currentPlayer->getUnitForMapCoordinates(position);
      if (unit == nullptr) continue;

      UnitType unitType = static_cast<UnitType>(unit->unitType);
      bool rightKind = GameUnit::isAir(unitType)  ? type == MapTileType::Airport :
                       GameUnit::isShip(unitType) ? type == MapTileType::Shipyard :
                       type != MapTileType::Airport && type != MapTileType::Shipyard;
      if (rightKind) healUnit(*unit, 2);
    }

    // Every own Assist heals each own unit next to it by 1, other Assists too.
    // A unit next to two Assists heals 2.
    for (uint8_t a = 0; a < currentPlayer->units.getCount(); a++) {
      GameUnit &assist = currentPlayer->units[a];
      if (assist.unitType != static_cast<uint8_t>(UnitType::Assist)) continue;

      for (uint8_t i = 0; i < currentPlayer->units.getCount(); i++) {
        GameUnit &unit = currentPlayer->units[i];
        int8_t dx = unit.mapPosX - assist.mapPosX;
        int8_t dy = unit.mapPosY - assist.mapPosY;
        if (dx * dx + dy * dy == 1) healUnit(unit, 1);
      }
    }

    // Against the AI the screen always shows player 1's view.
    AWPlayer &viewer = aiTurn ? player1 : *currentPlayer;

    // update player map
    updateMapForPlayer(viewer);

    // Draw map and HUD for new player
    arduboy.clear();

    // Calculate camera Position
    Point cursorPosition = cursorForTile(viewer.cursorIndex);

    // Calculate camera position
    cameraPosition = calculateCameraPosition(cursorPosition);
    cameraPosition.y -= mapOffsetY;

    // Draw map
    drawMapAtPosition(cameraPosition * -1);

    // Draw HUD
    drawHudForPlayer(viewer);

    // the new view wipes in from black
    if (wipe) makeScreenTransition(true);
    else arduboy.display();

    // against the AI the turn text pops up on the map
    if (startOnMap) showDialog(LOCA_yourTurn);

    // show player round
    doRoundOfPlayer(*currentPlayer, aiTurn);

    // check for win condition
    if (winner != NoWinner) {
      if (againstAI)
        showDialog((winner == MapTile::BelongsToPlayer1) ? LOCA_youWin : LOCA_youLose);
      else
        showDialog((winner == MapTile::BelongsToPlayer1) ? LOCA_player1Wins : LOCA_player2Wins);
      return;
    }

    // after 255 days the game ends in a draw
    if (daysPlayed == 255 && *currentPlayer == player2) {
      showDialog(LOCA_draw);
      return;
    }

    // switch players
    if (*currentPlayer == player1){
      currentPlayer = &player2;
    }
    else{
      currentPlayer = &player1;
      daysPlayed++;
    }
  }
}

void AWGame::doRoundOfPlayer(AWPlayer &currentPlayer, bool aiTurn){

  uint8_t scrollMultiplier = SCROLLSPEED_NORMAL;

  // Against the AI the screen always shows player 1's view.
  AWPlayer &viewer = aiTurn ? player1 : currentPlayer;
  viewingPlayer = &viewer;
  movingSide = playerIndex(currentPlayer);

  // the enemy the AI unit attacks after its walk, {-1,-1} for none
  Point aiTarget = {-1, -1};

  // Store cursor data
  Point cursorPosition = cursorForTile(viewer.cursorIndex);
  Point currentIndex = {0, 0};

  // tile the movement arrow currently leads to
  Point pathEnd = {-1, -1};

  // store additional helpers
  GameUnit *selectedUnit = nullptr;
  Point originalUnitPosition = {0, 0};

  // calculate mapsize in pixels, needed for camera stuff
  Point mapSizeInPixel = mapSize*TILE_SIZE;

  // stores the state of the round
  AWTurnState turnState = AWTurnState::Default;

  // The walk of the selected unit: where its sprite is (in map pixels),
  // how many frames it has walked and if it walks back to its start.
  Point walkPosition = {0, 0};
  uint8_t walkFrame = 0;
  bool walkBack = false;

  // False while an AI unit walks inside player 1's fog: it stays hidden.
  bool walkerSeen = true;

  // helper function to start the walk of the selected unit
  // This is a lambda function.  Also called anonymous function
  // It only exists inside this one certain method.
  // We do it this way because we will call it on different user interactions.
  // Always inlined: as a separate function it costs about 400 bytes of flash.
  auto walkSelectedUnit = [&](bool backwards) __attribute__((always_inline)) {

    // clear the markers
    unmarkUnitOnMap(nullptr);

    // A hidden enemy on the way stops the unit before it.
    // Count the enemies in sight, to see later if the walk uncovers one.
    if (!backwards) {
      ambushed = cutPathAtAmbush(selectedUnit);
      showAmbush = ambushed;
      enemiesSeen = countVisibleEnemies();
    }

    // hide the unit on the map, its sprite walks instead
    mapTileData[selectedUnit->mapPosY*mapSize.x+selectedUnit->mapPosX].hasUnit = 0;

    walkPosition.x = selectedUnit->mapPosX*TILE_SIZE;
    walkPosition.y = selectedUnit->mapPosY*TILE_SIZE;
    walkFrame = 0;
    walkBack = backwards;
    turnState = AWTurnState::UnitWalking;
  };

    // Game loop
    while(true){

        // wait for next frame
        if(!arduboy.nextFrame()) continue;

        // Handle Button Inputs
        arduboy.pollButtons();

        if (turnState == AWTurnState::UnitWalking) {
          if (walkFrame == pathLength*(TILE_SIZE/2)) {
            // arrived: the unit takes its new place on the map
            selectedUnit->mapPosX = walkPosition.x/TILE_SIZE;
            selectedUnit->mapPosY = walkPosition.y/TILE_SIZE;
            updateMapForPlayer(viewer);

            // No undo after an ambush or when the walk uncovered an enemy.
            // ponytail: compares counts, so losing sight of one enemy while
            // uncovering another keeps Cancel. Rare and harmless.
            canCancel = !ambushed && countVisibleEnemies() <= enemiesSeen;

            if (walkBack) {
              selectedUnit = nullptr;
              turnState = AWTurnState::Default;
            }
            else {
              turnState = AWTurnState::UnitMoved;
            }
          }
          else {
            // The unit walks 2 pixels per frame, 8 frames per tile.
            // Walking back takes the steps from the end, each one turned around.
            uint8_t step = walkFrame/(TILE_SIZE/2);
            uint8_t direction = walkBack ? (pathDirections[pathLength-1-step]+2)%4 : pathDirections[step];
            walkPosition.x += 2*((direction == 0) - (direction == 2));
            walkPosition.y += 2*((direction == 3) - (direction == 1));
            walkFrame++;
          }

          // An AI unit in player 1's fog stays hidden. Check the tile under its middle.
          if (aiTurn) {
            Point center = (walkPosition + Point{8, 8}) / TILE_SIZE;
            walkerSeen = !mapTileData[center.y*mapSize.x+center.x].showsFog;
          }

          // the cursor rides along, so the camera follows the unit
          if (walkerSeen) {
            cursorPosition.x = walkPosition.x-8;
            cursorPosition.y = walkPosition.y-8;
          }
        }
        else if (turnState == AWTurnState::UnitAttack) {
          // Step through the possible targets, one per button press:
          // right/down go forward, left/up go backward.
          int8_t direction = 0;
          if (arduboy.justPressed(RIGHT_BUTTON) || arduboy.justPressed(DOWN_BUTTON)) direction = 1;
          if (arduboy.justPressed(LEFT_BUTTON) || arduboy.justPressed(UP_BUTTON)) direction = -1;

          if (direction != 0) {
            Point nextPos = nextMarkedMapPosition(currentIndex, direction);
            if (nextPos.x >= 0) { // {-1,-1} means no marked position
              // move camera to tile
              cursorPosition = cursorForTile(nextPos);
            }
          }
        }
        else {
          // Scroll the map
          if (arduboy.pressed(DOWN_BUTTON))  cursorPosition.y += scrollMultiplier;
          if (arduboy.pressed(UP_BUTTON))    cursorPosition.y -= scrollMultiplier;
          if (arduboy.pressed(LEFT_BUTTON))  cursorPosition.x -= scrollMultiplier;
          if (arduboy.pressed(RIGHT_BUTTON)) cursorPosition.x += scrollMultiplier;

          if (arduboy.pressed(A_BUTTON)){
            scrollMultiplier = SCROLLSPEED_FAST;
          }
          else{
            scrollMultiplier = SCROLLSPEED_NORMAL;
          }
          if (arduboy.pressed(B_BUTTON)) {
            // snap to grid
            cursorPosition.x = (((int16_t)((cursorPosition.x+TILE_SIZE)/TILE_SIZE))*TILE_SIZE)-8;
            cursorPosition.y = (((int16_t)((cursorPosition.y+TILE_SIZE)/TILE_SIZE))*TILE_SIZE)-8;
          }
        }

        // Check for bounds
        //-8 because the cursor is 32x32 but a tile only 16x16.
        // If we want to center both we have to substract the half of their difference which is 8.
        if(cursorPosition.x < -8) cursorPosition.x = -8;
        if(cursorPosition.y < -8) cursorPosition.y = -8;

        // The 32 here again is the size of the cursor.
        if(cursorPosition.x > mapSizeInPixel.x-32+8) cursorPosition.x = mapSizeInPixel.x-32+8;
        if(cursorPosition.y > mapSizeInPixel.y-32+8) cursorPosition.y = mapSizeInPixel.y-32+8;

        // Calculate camera position
        Point cameraTarget = calculateCameraPosition(cursorPosition);
        cameraTarget.y -= mapOffsetY;

        // While a unit walks the camera glides after it instead of jumping.
        // In the AI's turn it stays where the last walk or animation left it.
        if (turnState == AWTurnState::UnitWalking) {
          if (walkerSeen) glideCameraTo(cameraTarget);
        }
        else if (!aiTurn) {
          cameraPosition = cameraTarget;
        }

        // calc currentIndex
        currentIndex.x = ((int16_t)(cursorPosition.x+TILE_SIZE))/TILE_SIZE;
        currentIndex.y = ((int16_t)(cursorPosition.y+TILE_SIZE))/TILE_SIZE;

        // Update the movement arrow when the cursor reaches a new tile.
        // This uses the screen buffer as scratch, so it must run before clear().
        if (turnState == AWTurnState::UnitSelected && !(currentIndex == pathEnd)) {
          pathEnd = currentIndex;
          if (mapTileData[currentIndex.y*mapSize.x+currentIndex.x].showSelection)
            buildPathTo(selectedUnit, currentIndex);
          else
            pathLength = 0;
        }

        // Start Drawing
        arduboy.clear();

        // Draw the map first
        drawMapAtPosition(cameraPosition * -1);

        // movement arrow from the selected unit to the cursor
        if (turnState == AWTurnState::UnitSelected)
          drawPath(selectedUnit, cameraPosition);

        // draw relevant cursor helper
        if ((turnState == AWTurnState::UnitSelected || turnState == AWTurnState::UnitWalking) && walkerSeen) {

          // draw the unit on top of the arrow, or where it walks
          drawUnit(walkPosition.x-cameraPosition.x, walkPosition.y-cameraPosition.y, currentPlayer == player1, selectedUnit->unitType, true);
        }

        // Draw the cursor on top. The AI plays without one.
        if (!aiTurn)
          sprites.drawPlusMask(cursorPosition.x-cameraPosition.x, cursorPosition.y-cameraPosition.y+1, gameCursorAnimation_plus_mask, (arduboy.frameCount/30)%2); // the y+1 looks more correct

        // Draw HUD
        drawHudForPlayer(viewer);

        // Draw everything
        arduboy.display();

        // no buttons while a unit walks
        if (turnState == AWTurnState::UnitWalking) continue;

        // An ambush shows its message once, when the unit has stopped
        // and the hidden enemy is in sight.
        if (turnState == AWTurnState::UnitMoved && showAmbush) {
          if (walkerSeen) showDialog(LOCA_ambush);
          showAmbush = false;
        }

        // The AI's turn: no buttons, it plays its units one by one.
        // Its thinking lives in AWGameAI.cpp. It borrows the screen buffer,
        // so it runs here, between display() and the next clear().
        if (aiTurn) {
          if (turnState == AWTurnState::UnitMoved) {
            // the unit arrived: attack or capture
            aiAct(selectedUnit, aiTarget, currentPlayer, ambushed);
            ambushed = false;
            updateMapForPlayer(player1);
            turnState = AWTurnState::Default;
            if (winner != NoWinner) return;
            continue;
          }

          // the next unit that hasn't acted yet
          selectedUnit = nullptr;
          for (uint8_t i = 0; i < currentPlayer.units.getCount(); i++) {
            if (currentPlayer.units[i].activated == GameUnit::UnitStateActive) {
              selectedUnit = &currentPlayer.units[i];
              break;
            }
          }

          // all units are done: buy new ones and end the turn
          if (selectedUnit == nullptr) {
            aiBuyUnits(currentPlayer);
            return;
          }

          Point start = {selectedUnit->mapPosX, selectedUnit->mapPosY};
          Point destination = aiPlanMove(selectedUnit, currentPlayer, aiTarget);
          if (destination == start) {
            turnState = AWTurnState::UnitMoved;
            continue;
          }

          // aiPlanMove built the path. An ambush may cut it short.
          walkSelectedUnit(false);

          // A walk that starts and ends in player 1's fog is not shown.
          // An ambush always is: it ends next to one of player 1's units.
          if (!ambushed &&
              mapTileData[start.y*mapSize.x+start.x].showsFog &&
              mapTileData[destination.y*mapSize.x+destination.x].showsFog) {
            walkPosition = destination*TILE_SIZE;
            walkFrame = pathLength*(TILE_SIZE/2);
          }
          continue;
        }

        // The unit has walked to its destination: ask what it does there.
        // The menu draws on top of the frame we just showed.
        if (turnState == AWTurnState::UnitMoved) {

          // Build the options for this unit and destination.
          // actions[] remembers what each visible option does.
          constexpr uint8_t ActionWait = 0;
          constexpr uint8_t ActionAttack = 1;
          constexpr uint8_t ActionCapture = 2;
          constexpr uint8_t ActionCancel = 3;

          UnitType unitType = static_cast<UnitType>(selectedUnit->unitType);
          UnitTraits traits = UnitTraits::traitsForUnitType(unitType);
          Point unitPosition = {selectedUnit->mapPosX, selectedUnit->mapPosY};
          bool hasMoved = !(unitPosition == originalUnitPosition);

          char_P* options[4];
          uint8_t actions[4];
          uint8_t numberOfOptions = 0;

          // Indirect units (range > 1) can't move and attack in the same turn.
          // Only offer the attack when a target is in range.
          if (traits.attackPower > 0 && !(traits.attackRange > 1 && hasMoved)) {
            Point unusedTarget;
            scanTargets(selectedUnit, unitPosition, currentPlayer, unusedTarget, true);
            if (nextMarkedMapPosition({-1, 0}).x >= 0) {
              options[numberOfOptions] = LOCA_attack;
              actions[numberOfOptions++] = ActionAttack;
            }
          }

          // an ambushed unit can't capture
          if (!ambushed && canUnitCaptureAtPosition(selectedUnit, unitPosition, currentPlayer)) {
            options[numberOfOptions] = LOCA_capture;
            actions[numberOfOptions++] = ActionCapture;
          }

          options[numberOfOptions] = LOCA_wait;
          actions[numberOfOptions++] = ActionWait;

          if (canCancel) {
            options[numberOfOptions] = LOCA_cancel;
            actions[numberOfOptions++] = ActionCancel;
          }

          int8_t selectedOption = showOptions(options, numberOfOptions);

          // A closes the menu. Without Cancel the menu just opens again.
          if (selectedOption < 0 && !canCancel) continue;
          uint8_t action = (selectedOption < 0) ? ActionCancel : actions[selectedOption];

          // the attack markers stay only when we attack
          if (action != ActionAttack) unmarkUnitOnMap(nullptr);

          switch (action) {

            ///////
            // WAIT
            case ActionWait:{
              // set unit to sleep
              selectedUnit->activated = GameUnit::UnitStateDisabled;
              updateMapForPlayer(currentPlayer);
              selectedUnit = nullptr;

              // update state
              turnState = AWTurnState::Default;
            }
            break;

            /////////
            // ATTACK
            case ActionAttack:{
              // prepare for attack
              turnState = AWTurnState::UnitAttack;

              // move the cursor to the first target (search from the start of the map)
              Point nextPos = nextMarkedMapPosition({-1, 0});
              cursorPosition = cursorForTile(nextPos);
            }
            break;

            //////////
            // CAPTURE
            case ActionCapture:{
              // the unit stays on the building and ends its turn
              selectedUnit->activated = GameUnit::UnitStateDisabled;

              captureBuildingWithUnit(selectedUnit, currentPlayer);

              updateMapForPlayer(currentPlayer);
              selectedUnit = nullptr;
              turnState = AWTurnState::Default;

              // capturing the enemy HQ ends the game
              if (winner != NoWinner) {
                currentPlayer.cursorIndex = currentIndex;
                return;
              }
            }
            break;

            /////////////////
            // CANCEL ACTION
            default:{

              // walk the unit back to its start
              walkSelectedUnit(true);
            }
          }
          // End of options

          // skip the button handling below, the menu used up this press
          continue;
        }

        //// Check for single button presses
        // THE A BUTTON
        if (arduboy.justPressed(A_BUTTON)) {

          // state machine for the turn
          switch (turnState) {
            default:
            case AWTurnState::Default:{
              // Don't react to A_BUTTON press
            }
            break; // End of default state

            case AWTurnState::UnitSelected:{
              // Ignore cancel button press when unit is selected
            }
            break; // End of unit selected state

            case AWTurnState::UnitAttack:{

              // remove the attack markers
              unmarkUnitOnMap(nullptr);

              // back to the menu, the unit stays where it walked to
              cursorPosition = cursorForTile({selectedUnit->mapPosX, selectedUnit->mapPosY});
              turnState = AWTurnState::UnitMoved;
            }
            break; // End attack state
          }

        }
        // THE B BUTTON
        if (arduboy.justPressed(B_BUTTON)){

          // state machine for the turn
          switch (turnState) {

            // Default State
            default:
            case AWTurnState::Default:{
              // check for tile
              MapTile currentMapTile = mapTileData[currentIndex.x + mapSize.x*currentIndex.y];
              MapTileType currentTileType = tileTypeAt(currentIndex.x, currentIndex.y);
              GameUnit *unitAtCursor = currentPlayer.getUnitForMapCoordinates(currentIndex);

              /////
              // check first for one of our units that hasn't acted yet
              if (unitAtCursor != nullptr && unitAtCursor->activated == GameUnit::UnitStateActive) {
                selectedUnit = unitAtCursor;

                // select unit on map
                originalUnitPosition = currentIndex;
                markUnitOnMap(selectedUnit);
                walkPosition = currentIndex*TILE_SIZE;
                pathLength = 0;
                pathEnd = {-1, -1};

                // update state
                turnState = AWTurnState::UnitSelected;
              }

              /////
              // Second check for shop that belongs to user
              else if (mapTileIndexIsShop(currentTileType) &&
              currentMapTile.buildingIsOccupied &&
              currentMapTile.buildingBelongsTo == MapTile::BelongsToPlayer &&
              !currentMapTile.hasUnit) {

                // check if there is space for a new unit
                if (currentPlayer.units.isFull()) {
                  showDialog(LOCA_Unit_limit_reached);
                }
                else{
                  // Show shop window
                  UnitType selectedUnit = showShopForBuildingAndPlayer(currentTileType, currentPlayer);

                  // react to the users choice if he buys a unit
                  if(selectedUnit != UnitType::None){
                    addNewUnit(currentPlayer, selectedUnit, currentIndex);
                    updateMapForPlayer(currentPlayer);
                  }
                }
              }

              /////
              // last show end turn option
              else {
                  char_P* options[2] = {LOCA_endTurn, LOCA_cancel};
                  uint8_t selectedOption = showOptions(options, 2);

                  if (selectedOption == 0) {
                    currentPlayer.cursorIndex = currentIndex;
                    return;
                  }
              }
            }
            break; // End of default state

            // A Unit is selected
            case AWTurnState::UnitSelected:{

              // get tile at current index
              MapTile currentTile = mapTileData[currentIndex.x + mapSize.x*currentIndex.y];

              // only continue if unit can move here
              if (currentTile.showSelection && selectedUnit != nullptr){

                // walk the unit along the arrow, the menu follows
                walkSelectedUnit(false);
              }
            }
            break; // End of Unit selected state

            // A Unit is attacking
            case AWTurnState::UnitAttack:{

              // only attack marked targets
              MapTile targetTile = mapTileData[currentIndex.x + mapSize.x*currentIndex.y];
              if (!targetTile.showSelection || selectedUnit == nullptr) break;

              // the attacking unit is done for this turn
              selectedUnit->activated = GameUnit::UnitStateDisabled;

              // the target markers go away for the battle
              unmarkUnitOnMap(nullptr);

              // Note: selectedUnit may be destroyed by the counterattack,
              // so it must not be used after this call.
              attackUnitAtPosition(selectedUnit, currentIndex, currentPlayer);
              selectedUnit = nullptr;

              updateMapForPlayer(currentPlayer);
              turnState = AWTurnState::Default;

              // destroying the last enemy unit ends the game
              if (winner != NoWinner) {
                currentPlayer.cursorIndex = currentIndex;
                return;
              }
            }
            break;

          }
        }
        // End button handling here
    }
}

void AWGame::showDialog(char_P *titleText){
  showBox(&titleText, 1, true);
}

int8_t AWGame::showOptions(char_P *options[], uint8_t count){
  return showBox(options, count, false);
}

int8_t AWGame::showBox(char_P * const options[], uint8_t count, bool isDialog){

  // frame for the box: a dialog is centered and closes with B only,
  // an option list sits top right and has a cursor.
  Rect frame;
  if (isDialog) {
    frame.width = strlen_P(options[0])*5+8;
    frame.height = 16;
    frame.x = (arduboy.width() - frame.width)/2;
    frame.y = 24;
  }
  else {
    frame.width = 54;
    frame.height = 6+count*7;
    frame.x = arduboy.width() - frame.width - 4;
    frame.y = 10;
  }
  uint8_t textX = frame.x + (isDialog ? 4 : 9);
  uint8_t textY = frame.y + (isDialog ? 6 : 4);

  // selected Index
  int8_t selectedIndex = 0;

  // dialog loop
  while (true) {

    // wait for next frame
    if(!arduboy.nextFrame()) continue;

    // Get input
    arduboy.pollButtons();

    // Close dialog and return with selected index.
    // A dialog only exits on B.
    if (arduboy.justPressed(B_BUTTON)) return selectedIndex;

    if (!isDialog) {
      // Cancel
      if (arduboy.justPressed(A_BUTTON)) return -1;
      if (arduboy.justPressed(DOWN_BUTTON)) selectedIndex++;
      if (arduboy.justPressed(UP_BUTTON)) selectedIndex--;

      // wrap and limit index
      if (selectedIndex < 0) selectedIndex = count-1;
      if (selectedIndex >= count) selectedIndex = 0;
    }

    // Drawing
    // Infobox
    arduboy.fillRoundRect(frame.x, frame.y, frame.width, frame.height, 5, BLACK);
    arduboy.fillRoundRect(frame.x + 1, frame.y + 1, frame.width-2, frame.height-3, 5, WHITE);

    // options
    for (uint8_t i = 0; i < count; i++) {
      tinyfont.setCursor(textX, textY + 7*i);
      tinyfont.print(AsFlashString(options[i]));
    }

    // cursor
    if (!isDialog) {
      tinyfont.setCursor(frame.x + 4, textY + 7*selectedIndex);
      tinyfont.print(AsFlashString(LOCA_Cursor));
    }

    arduboy.display();
  }
}

UnitType AWGame::showShopForBuildingAndPlayer(MapTileType building, AWPlayer &aPlayer){

  // data for the shop
  int8_t menuCursorIDX = 0;
  uint8_t numberOfUnits;
  const UnitType *buyableUnits; // array is stored in progmem!

  // helper for drawing
  UnitType unitToDraw;
  int8_t yOffset = 0;
  const uint8_t textPadding = 8;

  // check building type
  numberOfUnits = shopListForBuilding(building, buyableUnits);
  if (numberOfUnits == 0) return UnitType::None;

  // game Loop
  while (true) {
    // wait for next frame
    if(!arduboy.nextFrame()) continue;

    // Get input
    arduboy.pollButtons();

    if (arduboy.justPressed(DOWN_BUTTON)){
      menuCursorIDX++;
    }
    if (arduboy.justPressed(UP_BUTTON)){
      menuCursorIDX--;
    }
    // Exit on A button press
    if (arduboy.justPressed(A_BUTTON)){
      return UnitType::None;
    }
    // Buy Unit on button press
    if (arduboy.justPressed(B_BUTTON)){
      // get selected unit
      UnitType unitToBuy = static_cast<UnitType>(pgm_read_byte(buyableUnits+menuCursorIDX));
      uint8_t unitCosts = GameUnit::costsOfUnit(unitToBuy);

      // check if user has enough money to buy
      if (unitCosts <= aPlayer.money) {

        // substract the amount from the players funds
        aPlayer.money -= unitCosts;

        // return the unit
        return unitToBuy;
      }
    }

    // limit and wrap the cursor
    menuCursorIDX = max(menuCursorIDX, 0);
    menuCursorIDX = min(menuCursorIDX, numberOfUnits-1);
    yOffset = (4-menuCursorIDX)*textPadding;
    yOffset = min(yOffset, 0);

    // Drawing
    // Infobox
    arduboy.fillRoundRect(3, 9, arduboy.width()-6, arduboy.height()-10, 5, BLACK);
    arduboy.fillRoundRect(4, 10, arduboy.width()-8, arduboy.height()-13, 3, WHITE);

    // draw Units to buy
    for (uint8_t i = 0; i < numberOfUnits; i++) {
      // get unit
      unitToDraw = static_cast<UnitType>(pgm_read_byte(buyableUnits+i));

      // get unit costs
      uint8_t unitCosts = GameUnit::costsOfUnit(unitToDraw);
      bool canAffordUnit = (unitCosts <= aPlayer.money);

      // calc draw position
      int8_t yPos = 14 + i*textPadding + yOffset;

      // check if out of bounds
      if (yPos < 10 || yPos > arduboy.height()-8) continue;

      // set cursor
      // Intend the text if unit is buyable and selected
      tinyfont.setCursor(12 + (((menuCursorIDX == i) && canAffordUnit)?4:0), yPos);

      // The unitnames are stored in progmem inside an array wich is also in progmem.
      // This means we have to do two special things.
      // first get the element out of the array and the read the string from the array.
      // For that we cast the unittype to an int, since by design it is the index of
      // the string in the array.
      // Next we get the element out of the array. Remember that a c string is a pointer
      // to the first character and not the whole string. A pointer takes up 2 bytes which
      // is called a word and that's why we use pgm_read_word() to read it from there.
      // Now that we have the string we can pass it as a FlashString.
      tinyfont.print(AsFlashString(reinterpret_cast<char_P*>(pgm_read_word(&(LOCA_Unit_Names[static_cast<uint8_t>(unitToDraw)])))));

      // if unit is not affordable draw a x as a hint
      if (!canAffordUnit) {
          tinyfont.setCursor(66, yPos);
          tinyfont.print(F("x"));
      }
    }

    // draw cursor
    tinyfont.setCursor(6, 14 + menuCursorIDX*textPadding + yOffset);
    tinyfont.print(AsFlashString(LOCA_Cursor));

    // draw the sprite of the selected unit

    // get the unit
    unitToDraw = static_cast<UnitType>(pgm_read_byte(buyableUnits+menuCursorIDX));

    // Draw unit
    // we can safely cast the unittype since by design it is the index in the spritesheet
    drawUnit(90, 12, aPlayer == player1, static_cast<uint8_t>(unitToDraw), true);

    // draw Unit specs
    UnitTraits traits = UnitTraits::traitsForUnitType(unitToDraw);

    // Costs
    uint8_t unitPrice = GameUnit::costsOfUnit(unitToDraw);
    tinyfont.setCursor(76, 34);
    tinyfont.print(AsFlashString(LOCA_funds));
    tinyfont.setCursor(100 - ((unitPrice>100)?5:0), 34);
    printNumber(unitPrice*100); // Times 100 makes the value look larger but we still can store it in a byte :)

    // Attack
    tinyfont.setCursor(76, 42);
    tinyfont.print(AsFlashString(LOCA_Trait_attack));
    tinyfont.setCursor(115, 42);
    printNumber(traits.attackPower);

    // Speed
    tinyfont.setCursor(76, 50);
    tinyfont.print(AsFlashString(LOCA_Trait_speed));
    tinyfont.setCursor(115, 50);
    printNumber(traits.moveDistance);

    arduboy.display();
  }

  return UnitType::None;
}

void AWGame::drawHudForPlayer(AWPlayer &aPlayer){
  // Draw player, day and funds
  arduboy.fillRect(0, 0, 128, 7, BLACK);
  arduboy.fillRect(0, 0, 128, 6, WHITE);
  tinyfont.setCursor(1, 1);
  tinyfont.print((aPlayer == player1)?AsFlashString(LOCA_player1):AsFlashString(LOCA_player2));

  tinyfont.setCursor(46, 1);
  tinyfont.print(AsFlashString(LOCA_day));
  tinyfont.setCursor(66, 1);
  printNumber(daysPlayed);
  tinyfont.setCursor(86, 1);
  tinyfont.print(AsFlashString(LOCA_funds));
  tinyfont.setCursor(100, 1);
  printNumber(aPlayer.money*100);
}

Point AWGame::calculateCameraPosition(Point forCursorPosition){
  Point camera;

  // Calculate camera offset
  camera.x = forCursorPosition.x - (arduboy.width()-32)/2;
  camera.y = forCursorPosition.y - (arduboy.height()-32)/2;

  // calculate mapsize in pixels
  Point mapSizeInPixel = mapSize*TILE_SIZE;

  // Check for bounds
  if(camera.x < 0) camera.x = 0;
  if(camera.y < 1) camera.y = 1;
  if(camera.x > mapSizeInPixel.x-arduboy.width()) camera.x = mapSizeInPixel.x-arduboy.width();
  if(camera.y > mapSizeInPixel.y-arduboy.height()+mapOffsetY) camera.y = mapSizeInPixel.y-arduboy.height()+mapOffsetY;

  return camera;
}

void AWGame::loadMap(const unsigned char *newMapData){

  static_assert(MapDataHeaderSize == MAPDATAOFFSET_Main, "map header size mismatch");

  // Reset Buildings
  gameBuildings.clear();

  // handle map meta info
  // get size
  mapSize.x  = pgm_read_byte(newMapData+MAPDATAOFFSET_Size);
  mapSize.y  = pgm_read_byte(newMapData+MAPDATAOFFSET_Size+1);

  // refuse maps larger than the fixed buffer
  if (mapSize.x > MaxMapWidth || mapSize.y > MaxMapHeight) {
    mapSize = {0, 0};
    return;
  }
  mapData = newMapData;

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

  //// Load map data
  // Calculate map lenght
  uint16_t mapLenght = mapSize.x*mapSize.y;

  // reset all tile state
  memset(mapTileData, 0, sizeof(mapTileData));

  // The index in the loop will be stored here
  Point currentIndex;

  // Find the buildings
  for (uint16_t i = 0; i < mapLenght; i++) {

    MapTileType tileType = tileTypeAtIndex(i);

    // calc index
    currentIndex.x = i % mapSize.x;
    currentIndex.y = i / mapSize.x;

    // Check if it's a building
    if (mapTileIndexIsBuilding(tileType)) {
      GameBuilding building;

      // set upd building
      building.healthPoints = GameBuilding::CapturePoints;
      building.mapPosX = currentIndex.x;
      building.mapPosY = currentIndex.y;
      building.isOccupied = 0;
      building.belongsToPlayer = 0;
      building.buildingType = static_cast<uint8_t>(tileType);

      // Check for city
      if (tileType == MapTileType::City) {
        // check for ownership
        if (currentIndex == player1StartCityCoords) {
          building.isOccupied = 1;
          building.belongsToPlayer = MapTile::BelongsToPlayer1;
        }
        else if (currentIndex == player2StartCityCoords) {
          building.isOccupied = 1;
          building.belongsToPlayer = MapTile::BelongsToPlayer2;
        }
      }

      // Check for Factory
      if (tileType == MapTileType::Factory) {
        // check for ownership
        if (currentIndex == player1StartWorkshopCoords) {
          building.isOccupied = 1;
          building.belongsToPlayer = MapTile::BelongsToPlayer1;
        }
        else if (currentIndex == player2StartWorkshopCoords) {
          building.isOccupied = 1;
          building.belongsToPlayer = MapTile::BelongsToPlayer2;
        }
      }

      // Check for Headquarters
      if (tileType == MapTileType::P1HQ){
        building.isOccupied = 1;
        building.belongsToPlayer = MapTile::BelongsToPlayer1;
        player1.cursorIndex = currentIndex;
      }
      else if (tileType == MapTileType::P2HQ){
        building.isOccupied = 1;
        building.belongsToPlayer = MapTile::BelongsToPlayer2;
        player2.cursorIndex = currentIndex;
      }

      // add building to our global buildings
      gameBuildings.add(building);
    }

  }
}

void AWGame::updateMapForPlayer(AWPlayer &aPlayer){

  // fill map with fog
  clearMap(true);

  // add the enemies units
  AWPlayer &enemyPlayer = (aPlayer == player1)?player2:player1;
  for (uint8_t i = 0; i < enemyPlayer.units.getCount(); i++) {
    GameUnit unit = enemyPlayer.units[i];

    // get the corresponding map tile
    MapTile tile = mapTileData[unit.mapPosX+unit.mapPosY*mapSize.x];
    tile.hasUnit = 1;
    tile.unitBelongsTo = (enemyPlayer == player1)?MapTile::BelongsToPlayer1:MapTile::BelongsToPlayer2;
    mapTileData[unit.mapPosX+unit.mapPosY*mapSize.x] = tile;
  }

  // udpate the player units
  for (uint8_t i = 0; i < aPlayer.units.getCount(); i++) {
    GameUnit unit = aPlayer.units[i];

    // get the corresponding map tile
    MapTile tile = mapTileData[unit.mapPosX+unit.mapPosY*mapSize.x];
    tile.hasUnit = 1;
    tile.unitBelongsTo = (aPlayer == player1)?MapTile::BelongsToPlayer1:MapTile::BelongsToPlayer2;
    mapTileData[unit.mapPosX+unit.mapPosY*mapSize.x] = tile;

    // undo fog for units
    // Units can see same far as they can move.
    // get the traits of the unit
    UnitType unitType = static_cast<UnitType>(unit.unitType);
    UnitTraits traits = UnitTraits::traitsForUnitType(unitType);
    uint8_t sightRadius = traits.moveDistance;

    // add sight to unit if its on a mountain
    // Only infantry units can be on hills and mountains and they get a sight boost.
    if (tileTypeAt(unit.mapPosX, unit.mapPosY) == MapTileType::Mountain)
      sightRadius +=1;

    // remove the fog
    // The fog for a unit gets removed accordingly to it's sight.
    // Obstacles will conceal the things behind it so a unit can't
    // see through the enviroment.
    removeFogAtPositionRadiusAndPlayer({unit.mapPosX,unit.mapPosY}, sightRadius, aPlayer);
  }

  // udpate the  buildings
  for (uint8_t i = 0; i < gameBuildings.getCount(); i++) {
    GameBuilding building = gameBuildings[i];
    uint8_t thisPlayer = (aPlayer == player1)?MapTile::BelongsToPlayer1:MapTile::BelongsToPlayer2;

    // get the corresponding map tile
    MapTile tile = mapTileData[building.mapPosX+building.mapPosY*mapSize.x];

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

    // udpate maptile
    mapTileData[building.mapPosX+building.mapPosY*mapSize.x] = tile;

    // check if building belongs to player, because now we remove the fog of war calculations
    if (!(building.isOccupied && building.belongsToPlayer == thisPlayer)) continue;

    // remove fog of war
    removeFogAtPositionRadiusAndPlayer({building.mapPosX,building.mapPosY}, GameBuilding::buildingViewDistance, aPlayer, true);
  }
}

void AWGame::clearMap(bool withFog){
  // go trough the whole map
  for (int8_t y = 0; y < mapSize.y; y++) {
    for (int8_t x = 0; x < mapSize.x; x++) {

        // get the tile
        MapTile tile = mapTileData[y*mapSize.x+x];

        // turn fog on
        tile.showsFog = withFog?1:0;

        // disable selection
        tile.showSelection = 0;

        // remove unit info
        tile.hasUnit = 0;
        tile.unitBelongsTo = 0;

        // update tile
         mapTileData[y*mapSize.x+x] = tile;
    }
  }
}


int8_t AWGame::movementLeftAfter(MapTileType type, int8_t left){
  // movement malus for leaving this tile
  if (type == MapTileType::Forest || type == MapTileType::Hill)
    left--;
  else if (type == MapTileType::Mountain)
    left = min(2, left);

  // every step costs one
  return left-1;
}

uint8_t * AWGame::floodMovement(const GameUnit *aUnit){
  UnitType unitType = static_cast<UnitType>(aUnit->unitType);
  UnitTraits traits = UnitTraits::traitsForUnitType(unitType);
  uint8_t moveDistance = traits.moveDistance+1;

  // check if maptile is a street and give a small bonus
  if (mapTileIsStreet(tileTypeAt(aUnit->mapPosX, aUnit->mapPosY)))
    moveDistance += 1;

  // Movement range as a flood fill.
  // Every tile gets a number: the most moves the unit has left when it
  // arrives there (0 = it can't get there). The start tile gets the full
  // distance. Then we sweep the map again and again: every tile passes its
  // moves left, minus the cost of the step, to its neighbours. We stop when
  // a sweep changes nothing.
  //
  // The numbers need one byte per tile, 384 bytes on the largest map.
  // Instead of reserving that much RAM we borrow the screen buffer.
  // This is safe because we only get here after display() has sent the
  // picture to the screen, and the next frame starts with clear().
  // Only call this between display() and clear().
  // The caller reads the numbers right away. The next clear() wipes them.
  // The AI keeps a second map of numbers in the other half of the buffer.
  static_assert(2*MaxMapWidth*MaxMapHeight <= WIDTH*HEIGHT/8, "two maps do not fit in the screen buffer");
  return floodFrom({aUnit->mapPosX, aUnit->mapPosY}, unitType, moveDistance, arduboy.getBuffer());
}

uint8_t * AWGame::floodFrom(Point start, UnitType unitType, int8_t moves, uint8_t *remaining){
  // The start tile always gets the moves, even if this unit type couldn't
  // stand there: the AI floods outward from enemy positions.
  uint16_t mapLength = mapSize.x*mapSize.y;
  memset(remaining, 0, mapLength);
  remaining[start.y*mapSize.x+start.x] = moves;

  bool changed = true;
  while (changed) {
    changed = false;

    for (uint16_t i = 0; i < mapLength; i++) {
      if (remaining[i] == 0) continue;

      int8_t left = movementLeftAfter(tileTypeAtIndex(i), remaining[i]);
      if (left <= 0) continue;

      int8_t x = i % mapSize.x;
      int8_t y = i / mapSize.x;

      // right, up, left, down
      for (uint8_t direction = 0; direction < 4; direction++) {
        int8_t nx = x + (direction == 0) - (direction == 2);
        int8_t ny = y + (direction == 3) - (direction == 1);
        if (nx < 0 || nx >= mapSize.x || ny < 0 || ny >= mapSize.y) continue;

        uint16_t neighbour = ny*mapSize.x+nx;

        // Units can pass their own side's units, but enemies block the way.
        // Enemies hidden in the fog don't: running into one is an ambush.
        const MapTile &tile = mapTileData[neighbour];
        if (tile.hasUnit && tile.unitBelongsTo != movingSide && !tile.showsFog) continue;

        if (left > remaining[neighbour] && MapTile::canBeAccessedByUnit(tileTypeAtIndex(neighbour), unitType)) {
          remaining[neighbour] = left;
          changed = true;
        }
      }
    }
  }
  return remaining;
}

void AWGame::markUnitOnMap(const GameUnit *aUnit){
  uint8_t *remaining = floodMovement(aUnit);

  // show every reachable tile with no visible unit on it
  // (a hidden enemy's tile looks free, the player can't know)
  uint16_t mapLength = mapSize.x*mapSize.y;
  for (uint16_t i = 0; i < mapLength; i++) {
    const MapTile &tile = mapTileData[i];
    if (remaining[i] && !(tile.hasUnit && !tile.showsFog))
      mapTileData[i].showSelection = 1;
  }

  // the unit may also stay where it is
  mapTileData[aUnit->mapPosY*mapSize.x+aUnit->mapPosX].showSelection = 1;
}

void AWGame::buildPathTo(const GameUnit *aUnit, Point target){
  uint8_t *remaining = floodMovement(aUnit);
  pathLength = 0;

  // Walk back from the target: a tile we could have come from is a
  // neighbour whose leftover movement equals what the target got.
  int8_t x = target.x;
  int8_t y = target.y;
  while (x != aUnit->mapPosX || y != aUnit->mapPosY) {
    if (pathLength == MaxPathLength) { pathLength = 0; return; }

    uint8_t needed = remaining[y*mapSize.x+x];
    uint8_t direction = 0;
    for (; direction < 4; direction++) {
      // the neighbour we would have stepped from when moving in this direction
      int8_t px = x - (direction == 0) + (direction == 2);
      int8_t py = y - (direction == 3) + (direction == 1);
      if (px < 0 || px >= mapSize.x || py < 0 || py >= mapSize.y) continue;

      uint8_t r = remaining[py*mapSize.x+px];
      if (r && movementLeftAfter(tileTypeAt(px, py), r) >= (int8_t)needed) {
        x = px;
        y = py;
        break;
      }
    }
    if (direction == 4) { pathLength = 0; return; } // not reachable
    pathDirections[pathLength++] = direction;
  }

  // the steps were found from the end, so reverse them
  for (uint8_t i = 0; i < pathLength/2; i++) {
    uint8_t swap = pathDirections[i];
    pathDirections[i] = pathDirections[pathLength-1-i];
    pathDirections[pathLength-1-i] = swap;
  }
}

bool AWGame::cutPathAtAmbush(const GameUnit *aUnit){
  uint8_t width = mapSize.x;
  uint16_t index = aUnit->mapPosY*width+aUnit->mapPosX;

  // steps up to the last tile on the way where no unit stands
  uint8_t freeSteps = 0;

  for (uint8_t i = 0; i < pathLength; i++) {
    // one step: right, up, left or down
    uint8_t direction = pathDirections[i];
    if (direction == 0) index++;
    else if (direction == 1) index -= width;
    else if (direction == 2) index--;
    else index += width;
    MapTile tile = mapTileData[index];

    // An enemy on the way. Visible ones block the path, so this one was
    // hidden. Stop on the last free tile before it.
    if (tile.hasUnit && tile.unitBelongsTo != movingSide) {
      pathLength = freeSteps;
      return true;
    }
    if (!tile.hasUnit) freeSteps = i+1;
  }
  return false;
}

uint8_t AWGame::countVisibleEnemies(){
  uint8_t count = 0;
  uint16_t mapLength = mapSize.x*mapSize.y;
  for (uint16_t i = 0; i < mapLength; i++) {
    const MapTile &tile = mapTileData[i];
    if (tile.hasUnit && tile.unitBelongsTo != movingSide && !tile.showsFog) count++;
  }
  return count;
}

void AWGame::drawPath(const GameUnit *aUnit, Point camera){
  if (pathLength == 0) return;

  // Two passes: a black outline first, then the white arrow on top,
  // so the arrow stays readable on any terrain.
  for (uint8_t pass = 0; pass < 2; pass++) {
    uint8_t color = (pass == 0) ? BLACK : WHITE;
    uint8_t grow = (pass == 0) ? 1 : 0; // the outline is 1 pixel bigger

    // x and y are the tile centre: the line covers x-1 to x+2
    int16_t x = aUnit->mapPosX*TILE_SIZE + 7 - camera.x;
    int16_t y = aUnit->mapPosY*TILE_SIZE + 7 - camera.y;
    uint8_t direction = 0;
    for (uint8_t i = 0; i < pathLength; i++) {
      direction = pathDirections[i];
      // The last step stops at the edge of the target tile, so the
      // arrowhead tip ends a third into the tile.
      int8_t step = (i == pathLength-1) ? TILE_SIZE-9 : TILE_SIZE;
      int16_t nx = x + step*((direction == 0) - (direction == 2));
      int16_t ny = y + step*((direction == 3) - (direction == 1));
      int16_t left = min(x, nx);
      int16_t top = min(y, ny);
      uint8_t width = abs(nx - x);
      uint8_t height = abs(ny - y);
      arduboy.fillRect(left-1-grow, top-1-grow, width+4+2*grow, height+4+2*grow, color);
      x = nx;
      y = ny;
    }

    // Arrowhead in the last direction: 5 rows that get narrower
    // toward the tip, 10 pixels wide at the back.
    bool horizontal = (direction == 0 || direction == 2);
    bool forward = (direction == 0 || direction == 3);
    for (uint8_t row = 0; row < 5; row++) {
      int8_t half = 5 - row + grow;
      int8_t length = 1 + grow;
      // distance from the tile centre along the arrow (mirrored for left and up)
      int8_t along = forward ? 3+row : -2-row-grow;
      if (horizontal)
        arduboy.fillRect(x+along, y+1-half, length, 2*half, color);
      else
        arduboy.fillRect(x+1-half, y+along, 2*half, length, color);
    }
  }
}

void AWGame::unmarkUnitOnMap(const GameUnit *aUnit){

  //safety check
  if (aUnit != nullptr){
    // show again on map
    mapTileData[aUnit->mapPosY*mapSize.x+aUnit->mapPosX].hasUnit = 1;
  }

  // go trough the whole map and remove selection
  for (int8_t y = 0; y < mapSize.y; y++) {
    for (int8_t x = 0; x < mapSize.x; x++) {
        // get the tile
        mapTileData[y*mapSize.x+x].showSelection = 0;
    }
  }
}

uint8_t AWGame::scanTargets(const GameUnit *unit, Point position, AWPlayer &attackingPlayer, Point &target, bool markTargets){
  AWPlayer &enemy = (attackingPlayer == player1) ? player2 : player1;
  UnitType unitType = static_cast<UnitType>(unit->unitType);
  int8_t range = UnitTraits::traitsForUnitType(unitType).attackRange;
  uint8_t bestDamage = 0;

  // Walk every tile within the attack range (a diamond around the position)
  for (int8_t dy = -range; dy <= range; dy++) {
    int8_t span = range - abs(dy);
    for (int8_t dx = -span; dx <= span; dx++) {
      Point tile = {(int16_t)(position.x + dx), (int16_t)(position.y + dy)};

      // check for bounds
      if (tile.x < 0 || tile.x >= mapSize.x || tile.y < 0 || tile.y >= mapSize.y) continue;

      // only enemies we can see count, for the player and for the AI
      MapTile &mapTile = mapTileData[tile.y*mapSize.x+tile.x];
      if (mapTile.showsFog) continue;

      const GameUnit *enemyUnit = enemy.getUnitForMapCoordinates(tile);
      if (enemyUnit == nullptr) continue;

      // 0 means this unit can't attack that unit type
      uint8_t damage = GameUnit::calculateDamage(unitType, unit->healthPoints, static_cast<UnitType>(enemyUnit->unitType), tileTypeAt(tile.x, tile.y));
      if (damage == 0) continue;

      if (markTargets) mapTile.showSelection = 1;

      // finishing a unit off is worth more
      if (damage >= enemyUnit->healthPoints) damage += 5;
      if (damage > bestDamage) {
        bestDamage = damage;
        target = tile;
      }
    }
  }
  return bestDamage;
}


Point AWGame::nextMarkedMapPosition(Point currentPosition, int8_t direction){

    // iterate through the map
    for (int16_t i = (currentPosition.x+currentPosition.y*mapSize.x)+direction; i < (mapSize.x*mapSize.y) && i >= 0; i += direction) {

      // get maptile
      const MapTile tile = mapTileData[i];

      // return coordinates if found
      if (tile.showSelection)
        return {i%mapSize.x,i/mapSize.x};
    }

    return {-1,-1};
}

void AWGame::drawMapAtPosition(Point pos){

  // In this variable we store the
  Point drawPos;

  // draw the map
  for (int8_t y = 0; y < mapSize.y; y++) {
    for (int8_t x = mapSize.x-1; x >= 0; x--) { // draw the map from right to left helps with the markers

      drawPos.x = pos.x+x*TILE_SIZE;
      drawPos.y = pos.y+y*TILE_SIZE;

      // ignore if out of bounds
      if (drawPos.x <= -TILE_SIZE || drawPos.x >= arduboy.width() || drawPos.y <= -TILE_SIZE || drawPos.y >= (arduboy.height()+TILE_SIZE)) continue;

      // get the tile
      const MapTile tile = mapTileData[y*mapSize.x+x];
      const MapTileType tileType = tileTypeAt(x, y);

      // draw maptile
      sprites.drawSelfMasked(drawPos.x, drawPos.y, worldSprite, static_cast<uint8_t>(tileType));

      // check if tile below is the HQ
      if (y < mapSize.y-1) {
        const MapTileType tileTypeOfTileBelow = tileTypeAt(x, y+1);

        // If it's a headquarter draw upper half at the position above
        if (tileTypeOfTileBelow == MapTileType::P1HQ || tileTypeOfTileBelow == MapTileType::P2HQ) {
          sprites.drawOverwrite(drawPos.x, drawPos.y, worldSprite, (tileTypeOfTileBelow == MapTileType::P1HQ)?32:33);
        }
      }

      // draw fog
      if (tile.showsFog && !tile.showSelection) {
          sprites.drawErase(drawPos.x, drawPos.y, mapFOG_16x16, 0);
      }

      // Draw marker for buildings
      // we need to identify visually to whom the buiding belongs.
      // Since we have no colors to indicate this, we draw small markers at
      // the top right corner of a building.
      else if (mapTileIndexIsBuilding(tileType) && tileType != MapTileType::P1HQ && tileType != MapTileType::P2HQ) {
        if (tile.buildingIsOccupied){
          if(tile.buildingBelongsTo == MapTile::BelongsToPlayer){
            sprites.drawPlusMask(drawPos.x+10, drawPos.y-2, mapMarkers_plus_mask, 0);
          }
          else if(tile.buildingBelongsTo == MapTile::BelongsToEnemy){
            sprites.drawPlusMask(drawPos.x+10, drawPos.y-2, mapMarkers_plus_mask, 1);
          }
        }
      }

      // draw unit selection
      if (tile.showSelection) {
        sprites.drawErase(drawPos.x, drawPos.y, selectionAnimation, (arduboy.frameCount/10)%4);
      }

      // Draw Unit
      if(tile.hasUnit && !tile.showsFog){
        // look up the unit for its type, state and health
        AWPlayer &owner = (tile.unitBelongsTo == MapTile::BelongsToPlayer1) ? player1 : player2;
        const GameUnit *unit = owner.getUnitForMapCoordinates({x, y});
        if (unit == nullptr) continue;

        // Check if unit is active
        if (unit->activated == GameUnit::UnitStateActive) {
          // Draw sprite, animated
          drawUnit(drawPos.x, drawPos.y, &owner == &player1, unit->unitType, true);
        }
        else{
          // Don't animate if not active and draw a badge to top right.
          drawUnit(drawPos.x, drawPos.y, &owner == &player1, unit->unitType, false);
          sprites.drawPlusMask(drawPos.x+10, drawPos.y-2, mapMarkers_plus_mask, 2);
        }

        // Show remaining health in the bottom right corner once a unit is hurt.
        // Tinyfont does not clip: text near the right or bottom edge writes
        // past the screen buffer and corrupts RAM, so only draw the badge
        // where the digit fits completely.
        int16_t textX = drawPos.x+12;
        int16_t textY = drawPos.y+11;
        if (unit->healthPoints < GameUnit::UnitDefaultHealth &&
            textX >= 0 && textX <= arduboy.width()-4 && textY >= 0 && textY <= arduboy.height()-8) {
          arduboy.fillRect(drawPos.x+10, drawPos.y+9, 6, 7, BLACK);
          arduboy.fillRect(drawPos.x+11, drawPos.y+10, 5, 6, WHITE);
          tinyfont.setCursor(textX, textY);
          printNumber(unit->healthPoints);
        }
      }
    }
  }
}

void AWGame::makeScreenTransition(bool wipeIn){
  // The rows of 8 pixels slide sideways, even rows one way, odd rows the other.
  // Wiping out, the screen ends black. Wiping in, the frame in the buffer
  // slides in from black, the wipe out played backwards.
  // The buffer is not moved: each step sends a shifted copy to the display.
  uint8_t *dbuff = arduboy.getBuffer();

  for (uint8_t step = 0; step < arduboy.width(); step++) {
    uint8_t shift = wipeIn ? arduboy.width()-step : step+1;
    for (uint8_t y = 0; y < 8; y++) {
      for (uint8_t x = 0; x < arduboy.width(); x++) {
        // a source left of the screen wraps around to 128 and up, so the
        // one check below catches both edges
        uint8_t source = (y%2 == 0) ? x+shift : x-shift;
        Arduboy2Core::SPItransfer((source < arduboy.width()) ? dbuff[source+y*arduboy.width()] : 0);
      }
    }
    delay(1);
  }

  if (wipeIn) arduboy.display();
  else arduboy.clear();
}

GameBuilding * AWGame::getBuildingAtCoordinate(Point coordinate){

  // go through all buildings
  for (uint8_t i = 0; i < gameBuildings.getCount(); i++) {
    // when building is found return it
    if (gameBuildings[i].mapPosX == coordinate.x && gameBuildings[i].mapPosY == coordinate.y)
      return &gameBuildings[i];
  }

  return nullptr;
}

void AWGame::removeFogAtPositionRadiusAndPlayer(Point origin, uint8_t radius, AWPlayer &aPlayer, bool seeThrough){
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

void AWGame::castRayTo(Point origin, bool seeThrough, AWPlayer &aPlayer, int8_t xEnd, int8_t yEnd) {

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
      MapTile tile = mapTileData[x0+y0*mapSize.x];

      // update tile data
      if (forestLimit >= 0 || seeThrough)
        tile.showsFog = 0;

      mapTileData[x0+y0*mapSize.x] = tile;

      // ignore the origin
      if (currentDistance != 0 && !seeThrough){

        // check if there is an Obstacle
        if (mapTileIsOpaque(tileTypeAt(x0, y0)))
            break;

        // check if there is an enemy unit because we can't see through it
        if (tile.hasUnit && tile.unitBelongsTo == ((aPlayer == player1) ? MapTile::BelongsToPlayer2 : MapTile::BelongsToPlayer1))
          break;

        // check for forest
        if (tileTypeAt(x0, y0) == MapTileType::Forest)
          forestLimit--;
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

// ====================================================
// Combat and capturing

void AWGame::attackUnitAtPosition(GameUnit *attacker, Point target, AWPlayer &attackingPlayer){

  AWPlayer &defendingPlayer = (attackingPlayer == player1) ? player2 : player1;
  GameUnit *defender = defendingPlayer.getUnitForMapCoordinates(target);
  if (attacker == nullptr || defender == nullptr) return;

  UnitType attackerType = static_cast<UnitType>(attacker->unitType);
  UnitType defenderType = static_cast<UnitType>(defender->unitType);
  MapTileType defenderTile = tileTypeAt(target.x, target.y);
  MapTileType attackerTile = tileTypeAt(attacker->mapPosX, attacker->mapPosY);

  Point attackerPosition = {attacker->mapPosX, attacker->mapPosY};

  // show who fights whom, then the hit
  bool seen = playEffect(target, attackerPosition, EffectBlink);
  if (seen) flashScreen(3);

  uint8_t damage = GameUnit::calculateDamage(attackerType, attacker->healthPoints, defenderType, defenderTile);

  if (damage >= defender->healthPoints) {
    // defender destroyed: it flickers, then it's gone
    playEffect(target, target, EffectFlicker);
    defendingPlayer.units.removeAt(defender - defendingPlayer.units.getData());
    if (defendingPlayer.units.isEmpty()) winner = playerIndex(attackingPlayer);
    return;
  }
  defender->healthPoints -= damage;

  // Counterattack: only between direct units that are next to each other.
  UnitTraits attackerTraits = UnitTraits::traitsForUnitType(attackerType);
  UnitTraits defenderTraits = UnitTraits::traitsForUnitType(defenderType);
  uint8_t distance = abs(target.x - attacker->mapPosX) + abs(target.y - attacker->mapPosY);

  if (distance != 1 || attackerTraits.attackRange > 1 || defenderTraits.attackRange > 1) return;

  damage = GameUnit::calculateDamage(defenderType, defender->healthPoints, attackerType, attackerTile);
  if (damage == 0) return; // defender can't hit this unit type

  // the counterattack hits
  if (seen) flashScreen(2);

  if (damage >= attacker->healthPoints) {
    // attacker destroyed by the counterattack
    playEffect(attackerPosition, attackerPosition, EffectFlicker);
    attackingPlayer.units.removeAt(attacker - attackingPlayer.units.getData());
    if (attackingPlayer.units.isEmpty()) winner = playerIndex(defendingPlayer);
    return;
  }
  attacker->healthPoints -= damage;
}

bool AWGame::canUnitCaptureAtPosition(const GameUnit *unit, Point position, AWPlayer &aPlayer){
  if (unit == nullptr || !GameUnit::isInfantry(static_cast<UnitType>(unit->unitType))) return false;

  GameBuilding *building = getBuildingAtCoordinate(position);
  if (building == nullptr) return false;

  // can't capture what we already own
  return !(building->isOccupied && building->belongsToPlayer == playerIndex(aPlayer));
}

void AWGame::captureBuildingWithUnit(const GameUnit *unit, AWPlayer &capturingPlayer){
  Point position = {unit->mapPosX, unit->mapPosY};
  if (!canUnitCaptureAtPosition(unit, position, capturingPlayer)) return;

  GameBuilding *building = getBuildingAtCoordinate(position);

  // the cursor blinks on the building and the unit hops while it takes it over
  playEffect(position, position, EffectHop);

  // a hurt unit captures slower
  if (unit->healthPoints < building->healthPoints) {
    building->healthPoints -= unit->healthPoints;
    return;
  }

  // building taken
  building->isOccupied = 1;
  building->belongsToPlayer = playerIndex(capturingPlayer);
  building->healthPoints = GameBuilding::CapturePoints;

  MapTileType type = static_cast<MapTileType>(building->buildingType);
  if (type == MapTileType::P1HQ || type == MapTileType::P2HQ) {
    winner = playerIndex(capturingPlayer);
  }
  else if (capturingPlayer == *viewingPlayer) {
    // only the player's own captures get a popup, the enemy's show on the map
    showDialog(LOCA_captured);
  }
}

void AWGame::resetAbandonedCaptures(){
  for (uint8_t i = 0; i < gameBuildings.getCount(); i++) {
    GameBuilding &building = gameBuildings[i];
    if (building.healthPoints >= GameBuilding::CapturePoints) continue;

    Point position = {building.mapPosX, building.mapPosY};
    if (player1.getUnitForMapCoordinates(position) == nullptr &&
        player2.getUnitForMapCoordinates(position) == nullptr) {
      building.healthPoints = GameBuilding::CapturePoints;
    }
  }
}

// ====================================================
// Small helpers. Kept out of line on purpose: they are called from many
// places and inlining them everywhere costs more flash than the calls.

void AWGame::printNumber(uint16_t number){
  char digits[5];
  uint8_t count = 0;
  do {
    digits[count++] = '0' + number % 10;
    number /= 10;
  } while (number > 0);
  while (count > 0) tinyfont.print(digits[--count]);
}

MapTileType AWGame::tileTypeAtIndex(uint16_t index){
  return static_cast<MapTileType>(pgm_read_byte(mapData+MapDataHeaderSize+index));
}

MapTileType AWGame::tileTypeAt(int16_t x, int16_t y){
  return tileTypeAtIndex(y*mapSize.x+x);
}

void AWGame::drawUnit(int16_t x, int16_t y, bool firstPlayer, uint8_t unitType, bool animate){
  // A unit is drawn in two parts: rows 0-8 and rows 9-15.
  // When animated it bobs 1 pixel every 10 frames:
  // land units dip their upper part, the lower part stays on the ground,
  // planes float up as a whole, ships sink down as a whole.
  int8_t bob = animate ? (arduboy.frameCount/10)%2 : 0;
  int8_t lowerBob = 0;
  if (GameUnit::isAir(static_cast<UnitType>(unitType))) {
    bob = -bob;
    lowerBob = bob;
  }
  else if (GameUnit::isShip(static_cast<UnitType>(unitType))) {
    lowerBob = bob;
  }
  sprites.drawPlusMask(x, y+9+lowerBob, firstPlayer ? unitsA_lower_plus_mask : unitsB_lower_plus_mask, unitType);
  sprites.drawPlusMask(x, y+bob, firstPlayer ? unitsA_upper_plus_mask : unitsB_upper_plus_mask, unitType);
}

Point AWGame::cursorForTile(Point tile){
  // the 32x32 cursor is centered on the 16x16 tile
  return {(int16_t)(tile.x*TILE_SIZE-8), (int16_t)(tile.y*TILE_SIZE-8)};
}

void AWGame::addNewUnit(AWPlayer &owner, UnitType unitType, Point position){
  // A new unit has full health and waits until the next turn.
  GameUnit newUnit;
  newUnit.unitType = static_cast<uint8_t>(unitType);
  newUnit.healthPoints = GameUnit::UnitDefaultHealth;
  newUnit.payload = GameUnit::PayloadNone;
  newUnit.mapPosX = position.x;
  newUnit.mapPosY = position.y;
  newUnit.activated = GameUnit::UnitStateDisabled;
  owner.units.add(newUnit);
}

uint8_t AWGame::shopListForBuilding(MapTileType building, const UnitType *&buyableUnits){
  switch (building) {
    case MapTileType::Factory:
      buyableUnits = buyableUnitsAtFactory;
      return NumberOfBuyableUnitsAtFactory;
    case MapTileType::Airport:
      buyableUnits = buyableUnitsAtAirPort;
      return NumberOfBuyableUnitsAtAirport;
    case MapTileType::Shipyard:
      buyableUnits = buyableUnitsAtShipyard;
      return NumberOfBuyableUnitsAtShipyard;
    default:
      return 0;
  }
}

void AWGame::glideCameraTo(Point target){
  // moves the camera halfway to its target, then locks on once it is close
  auto glide = [](int16_t from, int16_t to) -> int16_t {
    int16_t difference = to - from;
    return (difference > 2 || difference < -2) ? from + difference/2 : to;
  };
  cameraPosition.x = glide(cameraPosition.x, target.x);
  cameraPosition.y = glide(cameraPosition.y, target.y);
}

// The capture bounce for each of the 60 frames of playEffect.
// Low 4 bits: the unit's height in pixels. Seven hops of 8 down to 1 pixel
// under one gravity, like a ball, touching the ground for one frame between.
// High 4 bits: 0 for still, else 1 + the frame of the building's shake in
// hopShake. Each landing starts a shake that runs on under the next hop,
// later landings start further in, so the shake fades.
static constexpr uint8_t hopFrame(uint8_t height, uint8_t shake){ return height | (shake << 4); }
static const uint8_t hopFrames[60] PROGMEM = {
  0,   // start on the ground
  4, 6, 7, 8, 7, 6, 4,   // hop 1, 8 pixels
  hopFrame(0,1),   // landing
  hopFrame(3,2), hopFrame(5,3), hopFrame(6,4), hopFrame(6,5), hopFrame(5,6), hopFrame(3,7),   // hop 2, 6 pixels
  hopFrame(0,1),   // landing
  hopFrame(3,2), hopFrame(4,3), hopFrame(5,4), hopFrame(4,5), hopFrame(3,6),   // hop 3, 5 pixels
  hopFrame(0,3),   // landing
  hopFrame(3,4), hopFrame(4,5), hopFrame(4,6), hopFrame(3,7),   // hop 4, 4 pixels
  hopFrame(0,4),   // landing
  hopFrame(2,5), hopFrame(3,6), hopFrame(3,7), hopFrame(2,8),   // hop 5, 3 pixels
  hopFrame(0,5),   // landing
  hopFrame(1,6), hopFrame(2,7), hopFrame(1,8),   // hop 6, 2 pixels
  hopFrame(0,6),   // landing
  hopFrame(1,7), hopFrame(1,8),   // hop 7, 1 pixel
  hopFrame(0,7),   // landing
  hopFrame(0,8), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0    // shake fades, rest (21 frames)
};

// The building's shake after a landing: sideways jolt in pixels.
// The first 2 frames also dip it 1 pixel.
static const int8_t hopShake[8] PROGMEM = {2, -2, 2, -1, 1, -1, 1, 0};

bool AWGame::playEffect(Point first, Point second, uint8_t effect){
  // About one second of animation for a battle, a destroyed unit or a capture.
  // EffectBlink: the cursor blinks on first and second.
  // EffectFlicker: the unit on first flickers before it is removed.
  // EffectHop: the cursor blinks on first and the unit there hops 3 times.
  // Tiles in the viewer's fog show nothing.
  MapTile &firstTile = mapTileData[first.y*mapSize.x+first.x];
  bool firstSeen = !firstTile.showsFog;
  if (!firstSeen && mapTileData[second.y*mapSize.x+second.x].showsFog) return false;

  // the camera glides to the tile, then the animation starts
  Point target = calculateCameraPosition(cursorForTile(firstSeen ? first : second));
  target.y -= mapOffsetY;

  uint8_t frame = 0;
  while (frame < 60) {
    while (!arduboy.nextFrame()) {}
    glideCameraTo(target);
    if (cameraPosition == target) frame++;

    // on for 10 frames, off for 10
    bool on = (frame/10)%2 == 0;
    if (effect == EffectFlicker) firstTile.hasUnit = on;

    // the hopping unit is drawn on its own, so the map leaves it out
    if (effect == EffectHop) firstTile.hasUnit = 0;

    arduboy.clear();
    drawMapAtPosition(cameraPosition * -1);

    if (effect == EffectHop) {
      // The capture bounce, one entry per frame (see hopFrames).
      uint8_t step = pgm_read_byte(hopFrames + frame);
      uint8_t height = step & 0x0F;
      Point position = first*TILE_SIZE - cameraPosition;
      if (step >> 4) {
        // the building shakes, the map drew it still underneath
        uint8_t shake = (step >> 4) - 1;
        int8_t dip = (shake < 2);
        int16_t x = position.x + (int8_t)pgm_read_byte(hopShake + shake);
        MapTileType type = tileTypeAt(first.x, first.y);
        sprites.drawOverwrite(x, position.y+dip, worldSprite, static_cast<uint8_t>(type));

        // the HQ's top half sits on the tile above
        if (type == MapTileType::P1HQ || type == MapTileType::P2HQ)
          sprites.drawOverwrite(x, position.y+dip-TILE_SIZE, worldSprite, (type == MapTileType::P1HQ)?32:33);

        // on the ground the unit dips with the building
        if (height == 0) position.y += dip;
      }
      AWPlayer &owner = (firstTile.unitBelongsTo == MapTile::BelongsToPlayer1) ? player1 : player2;
      drawUnit(position.x, position.y - height, &owner == &player1, owner.getUnitForMapCoordinates(first)->unitType, false);
    }

    for (uint8_t i = 0; on && effect != EffectFlicker && i < 2; i++) {
      Point tile = i ? second : first;
      if (mapTileData[tile.y*mapSize.x+tile.x].showsFog) continue;
      Point cursor = cursorForTile(tile) - cameraPosition;
      sprites.drawPlusMask(cursor.x, cursor.y+1, gameCursorAnimation_plus_mask, 0);
    }
    drawHudForPlayer(*viewingPlayer);
    arduboy.display();
  }

  // the hopping unit is back on the map
  if (effect == EffectHop) firstTile.hasUnit = 1;
  return true;
}

void AWGame::flashScreen(uint8_t times){
  // the hit: the screen turns inverted for a moment
  while (times--) {
    arduboy.invert(true);
    delay(70);
    arduboy.invert(false);
    delay(70);
  }
}
