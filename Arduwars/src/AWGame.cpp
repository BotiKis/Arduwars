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
  constexpr uint8_t yOffset = 44;

  while(true){

    // wait for next frame since we don't want to do things more than that.
    if(!arduboy.nextFrame()) continue;

    // Now we handle the button input of the arduboy
    arduboy.pollButtons();

    // move and wrap the cursor
    if (arduboy.justPressed(UP_BUTTON)) cursorIdx--;
    if (arduboy.justPressed(DOWN_BUTTON)) cursorIdx++;
    if (cursorIdx < 0) cursorIdx = count-1;
    if (cursorIdx >= count) cursorIdx = 0;

    if (arduboy.justPressed(B_BUTTON)) return cursorIdx;
    if (subtitle != nullptr && arduboy.justPressed(A_BUTTON)) return -1;

    // This clears the dispaly and makes everything black.
    arduboy.clear();

    // draw title logo
    sprites.drawSelfMasked(7, 0, arduwarsLogo, 0);

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
  }
}

// This method displays the Main menu to the player.
AWGameState AWGame::showMainMenu(){
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

  // load map
  loadMap(mapData);

  // In single player the AI plays player 2. Without the AI compiled in,
  // both modes are pass-and-play.
  runGame(mode == AWGameState::playSinglePlayer);
}

void AWGame::runGame(bool againstAI){

  AWPlayer *currentPlayer = &player1;

  // Game Loop
  while (true) {

    // show transition effect
    makeScreenTransition();

    // the AI plays player 2 in single player
#ifdef ARDUWARS_AI
    bool aiTurn = againstAI && *currentPlayer == player2;
#else
    constexpr bool aiTurn = false;
    (void)againstAI;
#endif

    // show dialog for player
    if (!aiTurn) showDialog((*currentPlayer == player1)?LOCA_player1:LOCA_player2);

    // activate all units
    for (uint8_t i = 0; i < currentPlayer->units.getCount(); i++) {
      currentPlayer->units[i].activated = GameUnit::UnitStateActive;
    }

    // capture progress is lost when the capturing unit left or was destroyed
    resetAbandonedCaptures();

    // Calculate Income for every City
    for (uint8_t i = 0; i < gameBuildings.getCount(); i++) {
      if(gameBuildings[i].buildingType == static_cast<uint8_t>(MapTileType::City) &&
      gameBuildings[i].isOccupied &&
      gameBuildings[i].belongsToPlayer == ((*currentPlayer == player1)?MapTile::BelongsToPlayer1:MapTile::BelongsToPlayer2)){
        currentPlayer->money += AWPlayer::BaseIncome;
      }
    }

    // update player map (against the AI the map always shows player 1's view)
    updateMapForPlayer(aiTurn ? player1 : *currentPlayer);

    // Draw map and HUD for new player
    arduboy.clear();

    // Calculate camera Position
    Point cursorPosition = cursorForTile(currentPlayer->cursorIndex);

    // Calculate camera position
    Point cameraPosition = calculateCameraPosition(cursorPosition);
    cameraPosition.y -= mapOffsetY;

    // Draw map
    drawMapAtPosition(cameraPosition * -1);

    // Draw HUD
    drawHudForPlayer(*currentPlayer);

    arduboy.display();

    // show player round
#ifdef ARDUWARS_AI
    if (aiTurn)
      doRoundOfAI(*currentPlayer);
    else
#endif
      doRoundOfPlayer(*currentPlayer);

    // check for win condition
    if (winner != NoWinner) {
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

void AWGame::doRoundOfPlayer(AWPlayer &currentPlayer){

  // Store cursor data
  // cursorPosition always sits on a tile (snap to grid). drawCursor is where
  // the cursor is drawn and eases toward cursorPosition for smooth movement.
  Point cursorPosition = cursorForTile(currentPlayer.cursorIndex);
  Point drawCursor = cursorPosition;
  uint8_t holdFrames = 0;
  Point cameraPosition = {0, 0};
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

  // helper function to reset  unit
  // This is a lambda function.  Also called anonymous function
  // It only exists inside this one certain method.
  // We do it this way because we will call it on different user interactions.
  auto resetSelectedUnit = [&cursorPosition, &selectedUnit, this, &originalUnitPosition]() {

    // cancel unit movement
    unmarkUnitOnMap(selectedUnit);

    // reset original unit
    selectedUnit->mapPosX = originalUnitPosition.x;
    selectedUnit->mapPosY = originalUnitPosition.y;
    selectedUnit = nullptr;

    // reset camera
    cursorPosition = cursorForTile(originalUnitPosition);
  };

  // moves a value halfway toward its target each frame
  auto ease = [](int16_t from, int16_t to) -> int16_t {
    int16_t difference = to - from;
    return from + ((difference > 1 || difference < -1) ? difference/2 : difference);
  };

    // Game loop
    while(true){

        // wait for next frame
        if(!arduboy.nextFrame()) continue;

        // Handle Button Inputs
        arduboy.pollButtons();

        if (turnState == AWTurnState::UnitAttack) {
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
        else if (arduboy.anyPressed(UP_BUTTON | DOWN_BUTTON | LEFT_BUTTON | RIGHT_BUTTON)) {
          // Move the cursor one tile per press. Holding a direction repeats
          // after a short delay (15 frames), then every 4 frames.
          holdFrames++;
          if (holdFrames == 1 || holdFrames == 16) {
            if (holdFrames == 16) holdFrames = 12;
            if (arduboy.pressed(DOWN_BUTTON))  cursorPosition.y += TILE_SIZE;
            if (arduboy.pressed(UP_BUTTON))    cursorPosition.y -= TILE_SIZE;
            if (arduboy.pressed(LEFT_BUTTON))  cursorPosition.x -= TILE_SIZE;
            if (arduboy.pressed(RIGHT_BUTTON)) cursorPosition.x += TILE_SIZE;
          }
        }
        else {
          holdFrames = 0;
        }

        // Check for bounds
        //-8 because the cursor is 32x32 but a tile only 16x16.
        // If we want to center both we have to substract the half of their difference which is 8.
        if(cursorPosition.x < -8) cursorPosition.x = -8;
        if(cursorPosition.y < -8) cursorPosition.y = -8;

        // The 32 here again is the size of the cursor.
        if(cursorPosition.x > mapSizeInPixel.x-32+8) cursorPosition.x = mapSizeInPixel.x-32+8;
        if(cursorPosition.y > mapSizeInPixel.y-32+8) cursorPosition.y = mapSizeInPixel.y-32+8;

        // smooth cursor movement
        drawCursor.x = ease(drawCursor.x, cursorPosition.x);
        drawCursor.y = ease(drawCursor.y, cursorPosition.y);

        // Calculate camera position
        cameraPosition = calculateCameraPosition(drawCursor);
        cameraPosition.y -= mapOffsetY;

        // calc currentIndex
        currentIndex.x = ((int16_t)(cursorPosition.x+TILE_SIZE))/TILE_SIZE;
        currentIndex.y = ((int16_t)(cursorPosition.y+TILE_SIZE))/TILE_SIZE;

        // Update the movement arrow when the cursor reaches a new tile.
        // This uses the screen buffer as scratch, so it must run before clear().
        if (turnState == AWTurnState::UnitSelected && selectedUnit != nullptr && !(currentIndex == pathEnd)) {
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
        if (turnState == AWTurnState::UnitSelected && selectedUnit != nullptr)
          drawPath(selectedUnit, cameraPosition);

        // Draw the cursor on top
        sprites.drawPlusMask(drawCursor.x-cameraPosition.x, drawCursor.y-cameraPosition.y+1, gameCursorAnimation_plus_mask, (arduboy.frameCount/30)%2); // the y+1 looks more correct

        // draw relevant cursor helper
        if (turnState == AWTurnState::UnitSelected && selectedUnit != nullptr) {

          // get correct sprite
          const unsigned char *unitSprite = nullptr;
          if(currentPlayer == player1)
            unitSprite = unitsA_plus_mask;
          else
            unitSprite = unitsB_plus_mask;

          // draw unit
          sprites.drawPlusMask(drawCursor.x-cameraPosition.x+8, drawCursor.y-cameraPosition.y+8, unitSprite, selectedUnit->unitType*2+(arduboy.frameCount/10)%2);
        }

        // Draw HUD
        drawHudForPlayer(currentPlayer);

        // Draw everything
        arduboy.display();

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

              // move unit to his original position
              selectedUnit->mapPosX = originalUnitPosition.x;
              selectedUnit->mapPosY = originalUnitPosition.y;

              // update map
              updateMapForPlayer(currentPlayer);

              // select unit on map
              markUnitOnMap(selectedUnit);

              // update state
              turnState = AWTurnState::UnitSelected;
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

                // Build the options for this unit and destination.
                // actions[] remembers what each visible option does.
                constexpr uint8_t ActionMove = 0;
                constexpr uint8_t ActionAttack = 1;
                constexpr uint8_t ActionCapture = 2;
                constexpr uint8_t ActionCancel = 3;
                constexpr uint8_t ActionAmbushed = 4;

                UnitType unitType = static_cast<UnitType>(selectedUnit->unitType);
                UnitTraits traits = UnitTraits::traitsForUnitType(unitType);
                bool hasMoved = !(currentIndex == originalUnitPosition);

                char_P* options[4];
                uint8_t actions[4];
                uint8_t numberOfOptions = 0;

                options[numberOfOptions] = LOCA_move;
                actions[numberOfOptions++] = ActionMove;

                // Indirect units (range > 1) can't move and attack in the same turn.
                if (traits.attackPower > 0 && !(traits.attackRange > 1 && hasMoved)) {
                  options[numberOfOptions] = LOCA_attack;
                  actions[numberOfOptions++] = ActionAttack;
                }

                if (canUnitCaptureAtPosition(selectedUnit, currentIndex, currentPlayer)) {
                  options[numberOfOptions] = LOCA_capture;
                  actions[numberOfOptions++] = ActionCapture;
                }

                options[numberOfOptions] = LOCA_cancel;
                actions[numberOfOptions++] = ActionCancel;

                int8_t selectedOption = showOptions(options, numberOfOptions);
                uint8_t action = (selectedOption < 0) ? ActionCancel : actions[selectedOption];

                // Ambush: the path runs into an enemy hidden in the fog.
                // The unit stops before it and its turn is over.
                Point stopAt;
                if (action != ActionCancel && cutPathAtAmbush(selectedUnit, stopAt)) {
                  unmarkUnitOnMap(nullptr);
                  animateUnitAlongPath(selectedUnit, currentPlayer, cameraPosition, false);
                  selectedUnit->mapPosX = stopAt.x;
                  selectedUnit->mapPosY = stopAt.y;
                  selectedUnit->activated = GameUnit::UnitStateDisabled;
                  updateMapForPlayer(currentPlayer);
                  showDialog(LOCA_ambush);
                  selectedUnit = nullptr;
                  turnState = AWTurnState::Default;
                  action = ActionAmbushed;
                }

                switch (action) {

                  // handled above
                  case ActionAmbushed:
                    break;

                  ///////
                  // MOVE
                  case ActionMove:{
                    // clear the markers and walk the unit along the arrow
                    unmarkUnitOnMap(nullptr);
                    animateUnitAlongPath(selectedUnit, currentPlayer, cameraPosition, false);

                    // update units position
                    selectedUnit->mapPosX = currentIndex.x;
                    selectedUnit->mapPosY = currentIndex.y;

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

                    // clear the markers and walk the unit along the arrow
                    unmarkUnitOnMap(nullptr);
                    animateUnitAlongPath(selectedUnit, currentPlayer, cameraPosition, false);

                    // update units position
                    selectedUnit->mapPosX = currentIndex.x;
                    selectedUnit->mapPosY = currentIndex.y;

                    // update map
                    updateMapForPlayer(currentPlayer);

                    // mark attack positions
                    Point unusedTarget;
                    scanTargets(selectedUnit, {selectedUnit->mapPosX,selectedUnit->mapPosY}, currentPlayer, unusedTarget, true);

                    // get first marked position if it exists (search from the start of the map)
                    Point nextPos = nextMarkedMapPosition({-1, 0});
                    if (nextPos.x >= 0) {
                      // move camera to tile
                      cursorPosition = cursorForTile(nextPos);
                    }
                    else {
                      // nothing in range: tell the player and go back to the selection
                      showDialog(LOCA_noTarget);
                      selectedUnit->mapPosX = originalUnitPosition.x;
                      selectedUnit->mapPosY = originalUnitPosition.y;
                      updateMapForPlayer(currentPlayer);
                      markUnitOnMap(selectedUnit);
                      turnState = AWTurnState::UnitSelected;
                    }
                  }
                  break;

                  //////////
                  // CAPTURE
                  case ActionCapture:{
                    // clear the markers and walk the unit along the arrow
                    unmarkUnitOnMap(nullptr);
                    animateUnitAlongPath(selectedUnit, currentPlayer, cameraPosition, false);

                    // move unit onto the building and end its turn
                    selectedUnit->mapPosX = currentIndex.x;
                    selectedUnit->mapPosY = currentIndex.y;
                    selectedUnit->activated = GameUnit::UnitStateDisabled;
                    updateMapForPlayer(currentPlayer);

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
                    turnState = AWTurnState::Default;

                    // unmark unit
                    unmarkUnitOnMap(selectedUnit);

                    // reset unit
                    resetSelectedUnit();
                    updateMapForPlayer(currentPlayer);
                  }
                }
                // End of options
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

    // Close and return with selected index
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

    // unitSprite
    const unsigned char *unitSprite = nullptr;

    // get correct sprite
    if(aPlayer == player1)
      unitSprite = unitsA_plus_mask;
    else
      unitSprite = unitsB_plus_mask;

    // Draw unit
    // we can safely cast the unittype since by design it is the index in the spritesheet
    sprites.drawPlusMask(90, 12, unitSprite, static_cast<uint8_t>(unitToDraw)*2+(arduboy.frameCount/10)%2);

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
  Point cameraPosition;

  // Calculate camera offset
  cameraPosition.x = forCursorPosition.x - (arduboy.width()-32)/2;
  cameraPosition.y = forCursorPosition.y - (arduboy.height()-32)/2;

  // calculate mapsize in pixels
  Point mapSizeInPixel = mapSize*TILE_SIZE;

  // Check for bounds
  if(cameraPosition.x < 0) cameraPosition.x = 0;
  if(cameraPosition.y < 1) cameraPosition.y = 1;
  if(cameraPosition.x > mapSizeInPixel.x-arduboy.width()) cameraPosition.x = mapSizeInPixel.x-arduboy.width();
  if(cameraPosition.y > mapSizeInPixel.y-arduboy.height()+mapOffsetY) cameraPosition.y = mapSizeInPixel.y-arduboy.height()+mapOffsetY;

  return cameraPosition;
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
  for (uint16_t i = 0; i < mapSize.x*mapSize.y; i++) {
    MapTile &tile = mapTileData[i];

    // turn fog on
    tile.showsFog = withFog?1:0;

    // disable selection
    tile.showSelection = 0;

    // remove unit info
    tile.hasUnit = 0;
    tile.unitBelongsTo = 0;
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

  // The result is kept in the screen buffer: callers must run between
  // display() and the next clear().
  return floodFrom({aUnit->mapPosX, aUnit->mapPosY}, unitType, moveDistance, arduboy.getBuffer(), ownerOf(aUnit));
}

uint8_t * AWGame::floodFrom(Point start, UnitType unitType, int8_t moves, uint8_t *remaining, uint8_t mover){
  // remaining[i] is the most movement left when entering tile i
  // (0 = not reachable). The start tile is always seeded, even if the unit
  // type couldn't stand there (the AI floods outward from enemy positions).
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
        // Units can pass their own side's units, but enemy units block the way.
        // Enemies hidden in the fog don't: running into one is an ambush.
        const MapTile &tile = mapTileData[neighbour];
        bool enemyThere = tile.hasUnit && tile.unitBelongsTo != mover && !tile.showsFog;
        if (left > remaining[neighbour] && !enemyThere && MapTile::canBeAccessedByUnit(tileTypeAtIndex(neighbour), unitType)) {
          remaining[neighbour] = left;
          changed = true;
        }
      }
    }
  }
  return remaining;
}

void AWGame::markUnitOnMap(const GameUnit *aUnit){
  // hide unit on map since it follow the cursor
  mapTileData[aUnit->mapPosY*mapSize.x+aUnit->mapPosX].hasUnit = 0;

  uint8_t *remaining = floodMovement(aUnit);

  // show every reachable tile with no visible unit on it
  // (a tile with a hidden enemy looks free, as the player can't tell)
  uint16_t mapLength = mapSize.x*mapSize.y;
  for (uint16_t i = 0; i < mapLength; i++) {
    const MapTile &tile = mapTileData[i];
    if (remaining[i] && !(tile.hasUnit && !tile.showsFog))
      mapTileData[i].showSelection = 1;
  }
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

void AWGame::drawPath(const GameUnit *aUnit, Point camera){
  // Two passes: a black outline first, then the white arrow on top,
  // so the arrow stays readable on any terrain.
  for (uint8_t pass = 0; pass < 2; pass++) {
    int16_t x = aUnit->mapPosX*TILE_SIZE + 7 - camera.x;
    int16_t y = aUnit->mapPosY*TILE_SIZE + 7 - camera.y;
    for (uint8_t i = 0; i < pathLength; i++) {
      uint8_t direction = pathDirections[i];
      int16_t nx = x + TILE_SIZE*((direction == 0) - (direction == 2));
      int16_t ny = y + TILE_SIZE*((direction == 3) - (direction == 1));
      int16_t left = min(x, nx);
      int16_t top = min(y, ny);
      uint8_t width = abs(nx - x);
      uint8_t height = abs(ny - y);
      if (pass == 0)
        arduboy.fillRect(left-2, top-2, width+6, height+6, BLACK);
      else
        arduboy.fillRect(left-1, top-1, width+4, height+4, WHITE);
      x = nx;
      y = ny;
    }
  }
}

void AWGame::animateUnitAlongPath(const GameUnit *aUnit, AWPlayer &owner, Point camera, bool onlyVisible){
  const unsigned char *unitSprite = (owner == player1) ? unitsA_plus_mask : unitsB_plus_mask;
  Point position = {(int16_t)(aUnit->mapPosX*TILE_SIZE - camera.x), (int16_t)(aUnit->mapPosY*TILE_SIZE - camera.y)};

  // 1 pixel per frame: 16 frames (about a quarter second) per tile
  int8_t x = aUnit->mapPosX;
  int8_t y = aUnit->mapPosY;
  for (uint8_t i = 0; i < pathLength; i++) {
    uint8_t direction = pathDirections[i];
    bool visible = !mapTileData[y*mapSize.x+x].showsFog;
    x += (direction == 0) - (direction == 2);
    y += (direction == 3) - (direction == 1);
    visible = visible || !mapTileData[y*mapSize.x+x].showsFog;

    // steps hidden in the fog of war are skipped
    if (onlyVisible && !visible) {
      position.x += TILE_SIZE*((direction == 0) - (direction == 2));
      position.y += TILE_SIZE*((direction == 3) - (direction == 1));
      continue;
    }

    for (uint8_t step = 0; step < TILE_SIZE; step++) {
      position.x += (direction == 0) - (direction == 2);
      position.y += (direction == 3) - (direction == 1);
      drawEffectFrame(camera, owner, nullptr, unitSprite, position, aUnit->unitType*2+(arduboy.frameCount/8)%2);
    }
  }
  pathLength = 0;
}

void AWGame::unmarkUnitOnMap(const GameUnit *aUnit){

  //safety check
  if (aUnit != nullptr){
    // show again on map
    mapTileData[aUnit->mapPosY*mapSize.x+aUnit->mapPosX].hasUnit = 1;
  }

  // go trough the whole map and remove selection
  for (uint16_t i = 0; i < mapSize.x*mapSize.y; i++)
    mapTileData[i].showSelection = 0;
}

uint8_t AWGame::scanTargets(const GameUnit *unit, Point position, AWPlayer &attackingPlayer, Point &target, bool markVisible){
  AWPlayer &enemy = (attackingPlayer == player1) ? player2 : player1;
  UnitType unitType = static_cast<UnitType>(unit->unitType);
  int8_t range = UnitTraits::traitsForUnitType(unitType).attackRange;
  uint8_t bestDamage = 0;

  // Walk every tile within the attack range (a diamond around the position)
  for (int8_t dy = -range; dy <= range; dy++) {
    int8_t span = range - abs(dy);
    for (int8_t dx = -span; dx <= span; dx++) {
      Point tile = {(int16_t)(position.x + dx), (int16_t)(position.y + dy)};
      if (tile.x < 0 || tile.x >= mapSize.x || tile.y < 0 || tile.y >= mapSize.y) continue;

      const GameUnit *enemyUnit = enemy.getUnitForMapCoordinates(tile);
      if (enemyUnit == nullptr) continue;

      // 0 means this unit can't attack that unit type
      uint8_t damage = GameUnit::calculateDamage(unitType, unit->healthPoints, static_cast<UnitType>(enemyUnit->unitType), tileTypeAt(tile.x, tile.y));
      if (damage == 0) continue;

      // the player can only target what they can see
      MapTile &mapTile = mapTileData[tile.y*mapSize.x+tile.x];
      if (markVisible && !mapTile.showsFog) mapTile.showSelection = 1;

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

        // get correct sprite
        const unsigned char *unitSprite = (tile.unitBelongsTo == MapTile::BelongsToPlayer1) ? unitsA_plus_mask : unitsB_plus_mask;

        // Check if unit is active
        if (unit->activated == GameUnit::UnitStateActive) {
          // Draw sprite
          // The *2 is because every unit in the spritesheet has two frames
          // The +(arduboy.frameCount/10)%2 part switches between these two frames
          // based on the current framecount.
          sprites.drawPlusMask(drawPos.x, drawPos.y, unitSprite, unit->unitType*2+(arduboy.frameCount/10)%2);
        }
        else{
          // Don't animate if not active and draw a badge to top right.
          // The *2 is because every unit in the spritesheet has two frames
          sprites.drawPlusMask(drawPos.x, drawPos.y, unitSprite, unit->unitType*2);
          sprites.drawPlusMask(drawPos.x+10, drawPos.y-2, mapMarkers_plus_mask, 2);
        }

        // Show remaining health in the bottom right corner once a unit is hurt.
        // Tinyfont does not clip: text near the right or bottom edge writes
        // past the screen buffer and corrupts RAM, so only draw the badge
        // where the digit fits completely (x <= 124, y <= 56 keeps both
        // display rows of an unaligned glyph inside the buffer).
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

void AWGame::makeScreenTransition(){
  uint8_t *dbuff = arduboy.getBuffer();

  for (uint8_t i = 0; i < arduboy.width(); i++) {
      for (uint8_t x = 0; x < arduboy.width(); x++) {
        for (uint8_t y = 0; y < 8; y++) {
          if (y%2==0) {
            if (x == arduboy.width()-1)
              dbuff[x+y*arduboy.width()] = 0;
            else
              dbuff[x+y*arduboy.width()] = dbuff[x+y*arduboy.width()+1];
          }
          else{
            uint8_t helperX = arduboy.width()-1-x;
            if (helperX == 0)
              dbuff[helperX+y*arduboy.width()] = 0;
            else
              dbuff[helperX+y*arduboy.width()] = dbuff[helperX+y*arduboy.width()-1];
          }
        }
      }

      arduboy.display();
      delay(1);
  }
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
  playEffect(target, attackerPosition, attackingPlayer, false);
  flashScreen(3);

  uint8_t damage = GameUnit::calculateDamage(attackerType, attacker->healthPoints, defenderType, defenderTile);

  if (damage >= defender->healthPoints) {
    // defender destroyed: it flickers, then it's gone
    playEffect(target, target, attackingPlayer, true);
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
  flashScreen(2);

  if (damage >= attacker->healthPoints) {
    // attacker destroyed by the counterattack
    playEffect(attackerPosition, attackerPosition, attackingPlayer, true);
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

  // the building blinks while it is being taken over
  if (!mapTileData[position.y*mapSize.x+position.x].showsFog)
    playEffect(position, position, capturingPlayer, false);

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
  else {
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

Point AWGame::cursorForTile(Point tile){
  // the 32x32 cursor is centered on the 16x16 tile
  return {(int16_t)(tile.x*TILE_SIZE-8), (int16_t)(tile.y*TILE_SIZE-8)};
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

#ifdef ARDUWARS_AI
// ====================================================
// Enemy AI
//
// Simple and greedy, built from the same pieces the player uses:
// every unit looks at all tiles it can reach this turn and picks the one
// with the best score - attacking for the most damage, capturing, or
// getting closer to its goal. Then it buys the most expensive useful unit
// it can afford at every free factory, airport and shipyard.
// The AI sees the whole map; the player only sees its moves where player 1
// has vision.

void AWGame::aiMoveUnit(GameUnit *unit, AWPlayer &ai){
  AWPlayer &enemy = (ai == player1) ? player2 : player1;
  UnitType unitType = static_cast<UnitType>(unit->unitType);
  bool infantry = GameUnit::isInfantry(unitType);
  bool indirect = UnitTraits::traitsForUnitType(unitType).attackRange > 1;
  Point start = {unit->mapPosX, unit->mapPosY};

  // Distance fields live in the second half of the screen buffer; the
  // first half holds this turn's movement range. A field value is the
  // movement left after travelling there, so higher means closer.
  uint8_t *field = arduboy.getBuffer() + MaxMapWidth*MaxMapHeight;
  constexpr int8_t FarAway = 127;

  // Goal: the closest enemy unit, or for infantry also the closest building
  // it could capture - closest by an actual route over terrain this unit can
  // cross, so land units don't head for targets across the water.
  floodFrom(start, unitType, FarAway, field, playerIndex(ai));
  Point goal = start;
  uint8_t goalCloseness = 0;
  uint8_t candidates = enemy.units.getCount() + gameBuildings.getCount();
  for (uint8_t i = 0; i < candidates; i++) {
    Point position;
    if (i < enemy.units.getCount()) {
      position = {enemy.units[i].mapPosX, enemy.units[i].mapPosY};
    }
    else {
      GameBuilding &building = gameBuildings[i - enemy.units.getCount()];
      position = {building.mapPosX, building.mapPosY};
      if (!infantry || !canUnitCaptureAtPosition(unit, position, ai)) continue;
    }

    // a target counts as reachable if we can get onto it or next to it
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
  floodFrom(goal, unitType, FarAway, field, playerIndex(ai));

  // Score every reachable free tile
  uint8_t *remaining = floodMovement(unit);
  Point best = start;
  Point bestTarget = {-1, -1};
  int16_t bestScore = -32000;

  for (uint16_t i = 0; i < mapSize.x*mapSize.y; i++) {
    if (remaining[i] == 0) continue;
    Point tile = {(int16_t)(i % mapSize.x), (int16_t)(i / mapSize.x)};
    bool isStart = (tile == start);
    if (!isStart && mapTileData[i].hasUnit) continue;

    // get closer to the goal along a real route, prefer cover
    int16_t score = 4*field[i] + mapTileDefenseBonus(tileTypeAtIndex(i));

    // capture
    if (infantry && canUnitCaptureAtPosition(unit, tile, ai)) {
      MapTileType type = tileTypeAtIndex(i);
      score += (type == MapTileType::P1HQ || type == MapTileType::P2HQ) ? 1000 : 200;
    }

    // attack (indirect units only without moving)
    Point target = {-1, -1};
    if (!indirect || isStart) {
      uint8_t damage = scanTargets(unit, tile, ai, target, false);
      if (damage > 0) score += 100 + 10*damage;
    }

    if (score > bestScore) {
      bestScore = score;
      best = tile;
      bestTarget = target;
    }
  }

  // Walk there. The camera follows only moves player 1 can see.
  Point camera = calculateCameraPosition(cursorForTile(best));
  camera.y -= mapOffsetY;
  if (!(best == start)) {
    buildPathTo(unit, best);
    mapTileData[start.y*mapSize.x+start.x].hasUnit = 0; // the walking sprite replaces it
    animateUnitAlongPath(unit, ai, camera, true);
    unit->mapPosX = best.x;
    unit->mapPosY = best.y;
  }
  unit->activated = GameUnit::UnitStateDisabled;
  updateMapForPlayer(player1);

  // Act
  if (bestTarget.x >= 0)
    attackUnitAtPosition(unit, bestTarget, ai);
  else if (infantry && canUnitCaptureAtPosition(unit, best, ai))
    captureBuildingWithUnit(unit, ai);
  updateMapForPlayer(player1);
}

void AWGame::doRoundOfAI(AWPlayer &ai){

  // Move every unit. A unit can be destroyed by a counterattack, which
  // shifts the list, so only advance when the unit is still there.
  uint8_t i = 0;
  while (i < ai.units.getCount() && winner == NoWinner) {
    uint8_t count = ai.units.getCount();
    if (ai.units[i].activated) aiMoveUnit(&ai.units[i], ai);
    if (ai.units.getCount() == count) i++;
  }
  if (winner != NoWinner) return;

  // Count infantry: keep at least 3 around for capturing
  uint8_t infantry = 0;
  for (uint8_t k = 0; k < ai.units.getCount(); k++)
    if (GameUnit::isInfantry(static_cast<UnitType>(ai.units[k].unitType))) infantry++;

  // Buy at every free shop we own
  for (uint8_t k = 0; k < gameBuildings.getCount(); k++) {
    GameBuilding &building = gameBuildings[k];
    MapTileType type = static_cast<MapTileType>(building.buildingType);
    Point position = {building.mapPosX, building.mapPosY};
    if (!building.isOccupied || building.belongsToPlayer != playerIndex(ai)) continue;
    if (player1.getUnitForMapCoordinates(position) || player2.getUnitForMapCoordinates(position)) continue;
    if (ai.units.isFull()) break;

    const UnitType *buyableUnits;
    uint8_t numberOfUnits = shopListForBuilding(type, buyableUnits);

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
#endif

void AWGame::addNewUnit(AWPlayer &owner, UnitType unitType, Point position){
  // A freshly built unit has full health and waits until the next turn.
  GameUnit newUnit;
  newUnit.unitType = static_cast<uint8_t>(unitType);
  newUnit.healthPoints = GameUnit::UnitDefaultHealth;
  newUnit.mapPosX = position.x;
  newUnit.mapPosY = position.y;
  newUnit.activated = GameUnit::UnitStateDisabled;
  newUnit.payload = GameUnit::PayloadNone;
  owner.units.add(newUnit);
}

void AWGame::playEffect(Point focus, Point other, AWPlayer &hudPlayer, bool flickerUnit){
  // About one second centred on focus: either the cursor blinking on focus
  // and other, or the unit on focus flickering (before it is removed).
  Point camera = calculateCameraPosition(cursorForTile(focus));
  camera.y -= mapOffsetY;
  MapTile &tile = mapTileData[focus.y*mapSize.x+focus.x];

  for (uint8_t frame = 0; frame < 60; frame++) {
    bool on = (frame / 10) % 2 == 0;
    if (flickerUnit) tile.hasUnit = on;

    Point highlight[2] = {focus, other};
    drawEffectFrame(camera, hudPlayer, (on && !flickerUnit) ? highlight : nullptr, nullptr, {0, 0}, 0);
  }
}

void AWGame::drawEffectFrame(Point camera, AWPlayer &hudPlayer, const Point *highlight, const unsigned char *sprite, Point spritePosition, uint8_t spriteFrame){
  // One frame of an animation: the map, optional cursors on two tiles,
  // an optional sprite (a walking unit), the HUD.
  while (!arduboy.nextFrame()) {}
  arduboy.clear();
  drawMapAtPosition(camera * -1);
  if (highlight != nullptr) {
    for (uint8_t i = 0; i < 2; i++) {
      Point cursor = cursorForTile(highlight[i]);
      sprites.drawPlusMask(cursor.x - camera.x, cursor.y - camera.y + 1, gameCursorAnimation_plus_mask, 0);
    }
  }
  if (sprite != nullptr) sprites.drawPlusMask(spritePosition.x, spritePosition.y, sprite, spriteFrame);
  drawHudForPlayer(hudPlayer);
  arduboy.display();
}

void AWGame::flashScreen(uint8_t times){
  while (times--) {
    arduboy.invert(true);
    delay(70);
    arduboy.invert(false);
    delay(70);
  }
}

uint8_t AWGame::ownerOf(const GameUnit *aUnit){
  // units live in their player's list
  const GameUnit *first = player1.units.getData();
  return (aUnit >= first && aUnit < first + player1.units.getCount()) ? MapTile::BelongsToPlayer1 : MapTile::BelongsToPlayer2;
}

Point AWGame::pathPosition(const GameUnit *aUnit, uint8_t steps){
  // where the unit is after walking the first steps of pathDirections
  Point position = {aUnit->mapPosX, aUnit->mapPosY};
  for (uint8_t i = 0; i < steps; i++) {
    uint8_t direction = pathDirections[i];
    position.x += (direction == 0) - (direction == 2);
    position.y += (direction == 3) - (direction == 1);
  }
  return position;
}

bool AWGame::cutPathAtAmbush(const GameUnit *aUnit, Point &stopAt){
  uint8_t mover = ownerOf(aUnit);
  for (uint8_t i = 1; i <= pathLength; i++) {
    Point position = pathPosition(aUnit, i);
    const MapTile &tile = mapTileData[position.y*mapSize.x+position.x];
    if (!tile.hasUnit || tile.unitBelongsTo == mover) continue;

    // An enemy on the way (visible ones are never on a path, so it was
    // hidden). Stop on the last tile before it that nobody stands on.
    pathLength = i-1;
    while (pathLength > 0) {
      Point stop = pathPosition(aUnit, pathLength);
      if (!mapTileData[stop.y*mapSize.x+stop.x].hasUnit) break;
      pathLength--;
    }
    stopAt = pathPosition(aUnit, pathLength);
    return true;
  }
  return false;
}
