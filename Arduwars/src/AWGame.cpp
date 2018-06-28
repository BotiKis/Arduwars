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

  // First we need to initialize our Arduboy instance by calling the usual methods.
  arduboy.begin();
  arduboy.setFrameRate(60);
  arduboy.initRandomSeed();
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
          startNewSinglePlayerGame();

          // If played, return to main menu
          this->gameState = AWGameState::showMainMenu;
          break;
        }
        case AWGameState::playMultiPlayer:{
          // start game
          startNewMultiplayerPlayerGame();

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


// This method displays the Main menu to the player.
AWGameState AWGame::showMainMenu(){

  // In this variable we will store the index of the cursor so
  // we know what the player has selected.
  int8_t cursorIdx = 0;
  constexpr uint8_t yOffset = 44;

  // Again a Game loop
  while(true){

    // wait for next frame since we don't want to do things more than that.
    // There may be isntances where we would do stuff between frames (e.g. smmulations, calculations, etc.)
    // but for this project we stick with this.
    if(!arduboy.nextFrame()) continue;

    // Now we handle the button input of the arduboy
    arduboy.pollButtons();

    // Here we limit and wrap the cursor so it cannot be larger than 2 nor smaller then 0.
    if (arduboy.justPressed(UP_BUTTON)){
      if(cursorIdx > 0)
        cursorIdx--;
      else
        cursorIdx = 2;
    }
    if (arduboy.justPressed(DOWN_BUTTON)){
      if(cursorIdx < 2)
        cursorIdx++;
      else
        cursorIdx = 0;
    }
    if (arduboy.justPressed(B_BUTTON)){

      // We do this here for random numbers we need later.
      // Random numbers are a complicated topic and won't be explained further here.
      arduboy.initRandomSeed();

      // React to users choice
      switch (cursorIdx) {
        case 0: return AWGameState::playSinglePlayer;
        case 1: return AWGameState::playMultiPlayer;
        case 2: {
          // toggle sound
          arduboy.audio.toggle();
          break;
        }
        default: return AWGameState::showMainMenu; // this default is not needed but it's safe to do this.
      }
    }

    // Now we start drawing the menu to the screen

    // This clears the dispaly and makes everything black.
    arduboy.clear();

    // draw title logo
    sprites.drawSelfMasked(7, 0, arduwarsLogo, 0);

    // Here we draw the menu text
    tinyfont.setTextColor(WHITE);

    tinyfont.setCursor(10, yOffset);
    tinyfont.print(AsFlashString(LOCA_SinglePlayer));

    tinyfont.setCursor(10, yOffset+8);
    tinyfont.print(AsFlashString(LOCA_MultiPlayer));

    tinyfont.setCursor(10, yOffset+16);
    tinyfont.print(AsFlashString(arduboy.audio.enabled()?LOCA_Sound_on:LOCA_Sound_off));

    tinyfont.setCursor(2, yOffset + 8*cursorIdx);
    tinyfont.print(AsFlashString(LOCA_Cursor));

    tinyfont.setTextColor(BLACK);

    // this one draws everything we have drawn to the actual screen.
    arduboy.display();

    // Now this loop wraps around and keeps repeating until the user selects
    // an menu item.
  }
}

// This method displays the map selection menu to the player.
const unsigned char * AWGame::showMapSelection(AWGameState aState){
  // In this variable we will store the index of the cursor so
  // we know what the player has selected.
  int8_t cursorIdx = 0;
  constexpr uint8_t yOffset = 44;

  // Again a Game loop
  while(true){

    // Skip the frame
    if(!arduboy.nextFrame()) continue;

    // Now we handle the button input of the arduboy
    arduboy.pollButtons();

    if (arduboy.justPressed(UP_BUTTON)){
      cursorIdx--;
    }
    if (arduboy.justPressed(DOWN_BUTTON)){
      cursorIdx++;
    }
    if (arduboy.justPressed(B_BUTTON)){

      // select map to users choice
      switch (cursorIdx) {
        case 0:{
          return mapData_3;
          break;
        }
        case 1:{
          return mapData_2;
          break;
        }
        case 2:{
          return mapData_1;
          break;
        }
        default: return nullptr;
      }
    }
    if (arduboy.justPressed(A_BUTTON)) {
      // When the A Button is pressed return nothing.
      return nullptr;
    }

    // Limit and wrap the index
    cursorIdx = (cursorIdx<0)?2:cursorIdx;
    cursorIdx = cursorIdx%3;

    // This clears the dispaly and makes everything black.
    arduboy.clear();

    // draw title logo
    sprites.drawSelfMasked(7, 0, arduwarsLogo, 0);

    // Set textcolor
    tinyfont.setTextColor(WHITE);

    // Draw the menu
    if (aState == AWGameState::playSinglePlayer) {
      tinyfont.setCursor(44, 20);
      tinyfont.print(AsFlashString(LOCA_SinglePlayer));
    }
    else if (aState == AWGameState::playMultiPlayer) {
      tinyfont.setCursor(36, 20);
      tinyfont.print(AsFlashString(LOCA_MultiPlayer));
    }

    tinyfont.setCursor(2, yOffset-8);
    tinyfont.print(AsFlashString(LOCA_BackWithA));

    tinyfont.setCursor(10, yOffset);
    tinyfont.print(AsFlashString(LOCA_mapSmall));
    tinyfont.setCursor(10, yOffset+8);
    tinyfont.print(AsFlashString(LOCA_mapMedium));
    tinyfont.setCursor(10, yOffset+16);
    tinyfont.print(AsFlashString(LOCA_mapBig));

    tinyfont.setCursor(2, yOffset + 8*cursorIdx);
    tinyfont.print(AsFlashString(LOCA_Cursor));

    tinyfont.setTextColor(BLACK);

    // this one draws everything we have drawn to the actual screen.
    arduboy.display();
  }
}

void AWGame::startNewSinglePlayerGame(){
  // reset day count;
  daysPlayed = 0;

  // reset players
  player1.reset();
  player2.reset();

  // shop map selection
  const unsigned char *mapData = showMapSelection(AWGameState::playSinglePlayer);

  // return if no map has been selected
  if (mapData == nullptr) return;

  // load map
  loadMap(mapData);

  // run game
  runSinglePlayerGame();
}

void AWGame::runSinglePlayerGame(){

  // The game loop
  while(true){

    if(!arduboy.nextFrame()) continue;

    arduboy.pollButtons();

    if (arduboy.justPressed(A_BUTTON)) {
      return;
    }

    // fill the screen white
    arduboy.fillScreen(WHITE);

    tinyfont.setCursor(1,1);

    arduboy.display();
  }
}

void AWGame::startNewMultiplayerPlayerGame(){
  // reset day count;
  daysPlayed = 0;

  // reset players
  player1.reset();
  player2.reset();

  // shop map selection
  const unsigned char *mapData = showMapSelection(AWGameState::playMultiPlayer);

  // return if no map has been selected
  if (mapData == nullptr) return;

  // load map
  loadMap(mapData);

  runMultiPlayerGame();
}

void AWGame::runMultiPlayerGame(){

  AWPlayer *currentPlayer = &player1;

  // Game Loop
  while (true) {

    // show transition effect
    makeScreenTransition();

    // show dialog for player
    showDialog((*currentPlayer == player1)?LOCA_player1:LOCA_player2);

    // activate all units
    for (uint8_t i = 0; i < currentPlayer->units.getCount(); i++) {
      currentPlayer->units[i].activated = GameUnit::UnitStateActive;
    }

    // Calculate Income for every City
    for (uint8_t i = 0; i < gameBuildings.getCount(); i++) {
      if(gameBuildings[i].buildingType == static_cast<uint8_t>(MapTileType::City) &&
      gameBuildings[i].isOccupied &&
      gameBuildings[i].belongsToPlayer == ((*currentPlayer == player1)?MapTile::BelongsToPlayer1:MapTile::BelongsToPlayer2)){
        currentPlayer->money += AWPlayer::BaseIncome;
      }
    }

    // update player map
    updateMapForPlayer(*currentPlayer);

    // Draw map and HUD for new player
    arduboy.clear();

    // Calculate camera Position
    Point cursorPosition = currentPlayer->cursorIndex*TILE_SIZE;
    cursorPosition.x -= 8;
    cursorPosition.y -= 8;

    // Calculate camera position
    Point cameraPosition = calculateCameraPosition(cursorPosition);
    cameraPosition.y -= mapOffsetY;

    // Draw map
    drawMapAtPosition(cameraPosition * -1);

    // Draw HUD
    drawHudForPlayer(*currentPlayer);

    arduboy.display();

    // show player round
    doRoundOfPlayer(*currentPlayer);

    // check for win condition
    if (daysPlayed == 255) {
      #warning implement win condition
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

  uint8_t scrollMultiplier = SCROLLSPEED_NORMAL;

  // Store cursor data
  Point cursorPosition = currentPlayer.cursorIndex*TILE_SIZE;
  cursorPosition.x -= 8;
  cursorPosition.y -= 8;
  Point cameraPosition = {0, 0};
  Point currentIndex = {0, 0};

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
    cursorPosition = originalUnitPosition*TILE_SIZE;
    cursorPosition.x -= 8;
    cursorPosition.y -= 8;
  };

    // Game loop
    while(true){

        // wait for next frame
        if(!arduboy.nextFrame()) continue;

        // Handle Button Inputs
        arduboy.pollButtons();

        if (arduboy.pressed(DOWN_BUTTON)){
          // state machine
          switch (turnState) {

            /////
            // Go through possible attack locations when unit is attacking
            case AWTurnState::UnitAttack:{
              // get next marked Pos if it exists
              Point nextPos = nextMarkedMapPosition(currentIndex, 1);
              if (nextPos.x > 0) { // This check if enough to verify a correct position
                // move camera to tile
                cursorPosition = nextPos*TILE_SIZE;
                cursorPosition.x -= 8;
                cursorPosition.y -= 8;
              }
            }
            break;

            ////
            // Default state, Normal Scroll map
            default:{
              cursorPosition.y += scrollMultiplier;
            }
          }
        }
        if (arduboy.pressed(UP_BUTTON)){
          // state machine
          switch (turnState) {

            /////
            // Go through possible attack locations when unit is attacking
            case AWTurnState::UnitAttack:{
              // get next marked Pos if it exists
              Point nextPos = nextMarkedMapPosition(currentIndex, -1);
              if (nextPos.x > 0) { // This check if enough to verify a correct position
                // move camera to tile
                cursorPosition = nextPos*TILE_SIZE;
                cursorPosition.x -= 8;
                cursorPosition.y -= 8;
              }
            }
            break;

            ////
            // Default state, Normal Scroll map
            default:{
              cursorPosition.y -= scrollMultiplier;
            }
          }
        }
        if (arduboy.pressed(LEFT_BUTTON)){
          // state machine
          switch (turnState) {

            /////
            // Go through possible attack locations when unit is attacking
            case AWTurnState::UnitAttack:{
              // get next marked Pos if it exists
              Point nextPos = nextMarkedMapPosition(currentIndex, -1);
              if (nextPos.x > 0) { // This check if enough to verify a correct position
                // move camera to tile
                cursorPosition = nextPos*TILE_SIZE;
                cursorPosition.x -= 8;
                cursorPosition.y -= 8;
              }

            }
            break;

            ////
            // Default state, Normal Scroll map
            default:{
              cursorPosition.x -= scrollMultiplier;
            }
          }
        }
        if (arduboy.pressed(RIGHT_BUTTON)){
          // state machine
          switch (turnState) {

            /////
            // Go through possible attack locations when unit is attacking
            case AWTurnState::UnitAttack:{
              // get next marked Pos if it exists
              Point nextPos = nextMarkedMapPosition(currentIndex, 1);
              if (nextPos.x > 0) { // This check if enough to verify a correct position
                // move camera to tile
                cursorPosition = nextPos*TILE_SIZE;
                cursorPosition.x -= 8;
                cursorPosition.y -= 8;
              }
            }
            break;

            // Default state, Normal Scroll map
            default:{
              cursorPosition.x += scrollMultiplier;
            }
          }
        }
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

        // Check for bounds
        //-8 because the cursor is 32x32 but a tile only 16x16.
        // If we want to center both we have to substract the half of their difference which is 8.
        if(cursorPosition.x < -8) cursorPosition.x = -8;
        if(cursorPosition.y < -8) cursorPosition.y = -8;

        // The 32 here again is the size of the cursor.
        if(cursorPosition.x > mapSizeInPixel.x-32+8) cursorPosition.x = mapSizeInPixel.x-32+8;
        if(cursorPosition.y > mapSizeInPixel.y-32+8) cursorPosition.y = mapSizeInPixel.y-32+8;

        // Calculate camera position
        cameraPosition = calculateCameraPosition(cursorPosition);
        cameraPosition.y -= mapOffsetY;

        // calc currentIndex
        currentIndex.x = ((int16_t)(cursorPosition.x+TILE_SIZE))/TILE_SIZE;
        currentIndex.y = ((int16_t)(cursorPosition.y+TILE_SIZE))/TILE_SIZE;

        // Start Drawing
        arduboy.clear();

        // Draw the map first
        drawMapAtPosition(cameraPosition * -1);

        // Draw the cursor on top
        sprites.drawPlusMask(cursorPosition.x-cameraPosition.x, cursorPosition.y-cameraPosition.y+1, gameCursorAnimation_plus_mask, (arduboy.frameCount/30)%2); // the y+1 looks more correct

        // draw relevant cursor helper
        if (turnState == AWTurnState::UnitSelected && selectedUnit != nullptr) {

          // get correct sprite
          const unsigned char *unitSprite = nullptr;
          if(currentPlayer == player1)
            unitSprite = unitsA_plus_mask;
          else
            unitSprite = unitsB_plus_mask;

          // draw unit
          sprites.drawPlusMask(cursorPosition.x-cameraPosition.x+8, cursorPosition.y-cameraPosition.y+8, unitSprite, selectedUnit->unitType*2+(arduboy.frameCount/10)%2);
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
              MapTileType currentTileType = static_cast<MapTileType>(currentMapTile.tileID);

              /////
              // check first for Unit
              if (currentMapTile.hasUnit &&
                currentMapTile.unitBelongsTo == ((currentPlayer == player1) ? MapTile::BelongsToPlayer1 : MapTile::BelongsToPlayer2) &&
                currentMapTile.unitIsActive == GameUnit::UnitStateActive) {
                // get unit
                selectedUnit = currentPlayer.getUnitForMapCoordinates({currentIndex.x,currentIndex.y});
                if (selectedUnit != nullptr){
                  // select unit on map
                  originalUnitPosition = currentIndex;
                  markUnitOnMap(selectedUnit);

                  // update state
                  turnState = AWTurnState::UnitSelected;
                }
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
                    GameUnit newUnit = GameUnit();

                    // init unit
                    newUnit.unitType = static_cast<uint8_t>(selectedUnit);
                    newUnit.mapPosX = currentIndex.x;
                    newUnit.mapPosY = currentIndex.y;
                    newUnit.activated = GameUnit::UnitStateDisabled;

                    currentPlayer.units.add(newUnit);
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

                // show options box
                char_P* options[3] = {LOCA_move, LOCA_attack, LOCA_cancel};
                switch (showOptions(options, 3)) {

                  ///////
                  // MOVE
                  case 0:{
                    // unmark unit
                    unmarkUnitOnMap(selectedUnit);

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
                  case 1:{
                    // prepare for attack
                    turnState = AWTurnState::UnitAttack;

                    // unmark unit
                    unmarkUnitOnMap(selectedUnit);

                    // update units position
                    selectedUnit->mapPosX = currentIndex.x;
                    selectedUnit->mapPosY = currentIndex.y;

                    // update map
                    updateMapForPlayer(currentPlayer);

                    // get units traits
                    UnitType unitType = static_cast<UnitType>(selectedUnit->unitType);
                    UnitTraits traits = UnitTraits::traitsForUnitType(unitType);

                    // mark attack positions
                    markPositionForAttack({selectedUnit->mapPosX,selectedUnit->mapPosY}, traits.attackRange+1, static_cast<UnitType>(selectedUnit->unitType), currentPlayer);

                    // get first marked position if it exists
                    Point nextPos = nextMarkedMapPosition(currentIndex);
                    if (nextPos.x > 0) { // This check if enough to verify a correct position
                      // move camera to tile
                      cursorPosition = nextPos*TILE_SIZE;
                      cursorPosition.x -= 8;
                      cursorPosition.y -= 8;
                    }
                  }

                  /////////////////
                  // CANCEL ACTION
                  break;
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

            }
            break;

          }
        }
        // End button handling here
    }
}

void AWGame::showDialog(char_P *titleText){

  // frame for the dialog
  Rect frame;

  frame.width = strlen_P(titleText)*5+8;
  frame.height = 16;
  frame.x = (arduboy.width() - frame.width)/2;
  frame.y = 24;

  Point textPos;
  textPos.x = (arduboy.width() - strlen_P(titleText)*5)/2;
  textPos.y = frame.y+6;

  // dialog loop
  while (true) {

    // wait for next frame
    if(!arduboy.nextFrame()) continue;

    // Get input
    arduboy.pollButtons();

    // Exit on B
    if (arduboy.justPressed(B_BUTTON)){
      return;
    }

    // Drawing
    // Infobox
    arduboy.fillRoundRect(frame.x, frame.y, frame.width, frame.height, 5, BLACK);
    arduboy.fillRoundRect(frame.x + 1, frame.y + 1, frame.width-2, frame.height-3, 5, WHITE);

    // OK Button
    tinyfont.setCursor(textPos.x, textPos.y);
    tinyfont.print(AsFlashString(titleText));

    arduboy.display();
  }
}

int8_t AWGame::showOptions(char_P *options[], uint8_t count){

  // frame for the dialog
  Rect frame;

  frame.width = 54;
  frame.height = 6+count*7;
  frame.x = arduboy.width() - frame.width - 4;
  frame.y = 10;

  // selected Index
  int8_t selectedIndex = 0;

  // dialog loop
  while (true) {

    // wait for next frame
    if(!arduboy.nextFrame()) continue;

    // Get input
    arduboy.pollButtons();

    // Cancel
    if (arduboy.justPressed(A_BUTTON)){
      return -1;
    }
    // Close dialog and return with selected index
    if (arduboy.justPressed(B_BUTTON)){
      return selectedIndex;
    }
    if (arduboy.justPressed(DOWN_BUTTON)){
      selectedIndex++;
    }  // Exit on button press
    if (arduboy.justPressed(UP_BUTTON)){
      selectedIndex--;
    }
    // wrap and limit index
    selectedIndex = (selectedIndex<0)?(count-1):selectedIndex;
    selectedIndex = selectedIndex%count;

    // Drawing
    // Infobox
    arduboy.fillRoundRect(frame.x, frame.y, frame.width, frame.height, 5, BLACK);
    arduboy.fillRoundRect(frame.x + 1, frame.y + 1, frame.width-2, frame.height-3, 5, WHITE);

    // options
    for (uint8_t i = 0; i < count; i++) {
      tinyfont.setCursor(frame.x + 9, frame.y + 4 + 7*i);
      tinyfont.print(AsFlashString(options[i]));
    }

    // cursor
    tinyfont.setCursor(frame.x + 4, frame.y + 4 + 7*selectedIndex);
    tinyfont.print(AsFlashString(LOCA_Cursor));

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
  switch (building) {
    case MapTileType::Factory:{
      numberOfUnits = NumberOfBuyableUnitsAtFactory;
      buyableUnits = buyableUnitsAtFactory;
      break;
    }
    case MapTileType::Airport:{
      numberOfUnits = NumberOfBuyableUnitsAtAirport;
      buyableUnits = buyableUnitsAtAirPort;
      break;
    }
    case MapTileType::Shipyard:{
      numberOfUnits = NumberOfBuyableUnitsAtShipyard;
      buyableUnits = buyableUnitsAtShipyard;
      break;
    }
    default: return UnitType::None;
  }

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
      tinyfont.print(AsFlashString(pgm_read_word(&(LOCA_Unit_Names[static_cast<uint8_t>(unitToDraw)]))));

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
    tinyfont.print(unitPrice*100); // Times 100 makes the value look larger but we still can store it in a byte :)

    // Attack
    tinyfont.setCursor(76, 42);
    tinyfont.print(AsFlashString(LOCA_Trait_attack));
    tinyfont.setCursor(115, 42);
    tinyfont.print(traits.attackPower);

    // Speed
    tinyfont.setCursor(76, 50);
    tinyfont.print(AsFlashString(LOCA_Trait_speed));
    tinyfont.setCursor(115, 50);
    tinyfont.print(traits.moveDistance);

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
  tinyfont.print(daysPlayed);
  tinyfont.setCursor(86, 1);
  tinyfont.print(AsFlashString(LOCA_funds));
  tinyfont.setCursor(100, 1);
  tinyfont.print(aPlayer.money*100);
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

void AWGame::loadMap(const unsigned char *mapData){

  // clear old map Data
  if (mapTileData != nullptr) {
    delete [] mapTileData;
    mapTileData = nullptr;
  }

  // Reset Buildings
  gameBuildings.clear();

  // handle map meta info
  // get size
  mapSize.x  = pgm_read_byte(mapData+MAPDATAOFFSET_Size);
  mapSize.y  = pgm_read_byte(mapData+MAPDATAOFFSET_Size+1);

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

  // create new array of maptiles
  mapTileData = new MapTile[mapLenght];

  // The index in the loop will be stored here
  Point currentIndex;

  // Populate array from map data
  for (uint16_t i = 0; i < mapLenght; i++) {

    // get Tile Data
    mapTileData[i].tileID = pgm_read_byte(mapData+MAPDATAOFFSET_Main+i);
    mapTileData[i].buildingIsOccupied = 0;
    mapTileData[i].buildingBelongsTo = 0;
    mapTileData[i].hasUnit = 0;
    mapTileData[i].unitBelongsTo = 0;

    MapTileType tileType = static_cast<MapTileType>(mapTileData[i].tileID);

    // calc index
    currentIndex.x = i % mapSize.x;
    currentIndex.y = i / mapSize.x;

    // Check if it's a building
    if (mapTileIndexIsBuilding(tileType)) {
      GameBuilding building = GameBuilding();

      // set upd building
      building.mapPosX = currentIndex.x;
      building.mapPosY = currentIndex.y;
      building.isOccupied = 0;
      building.belongsToPlayer = 0;
      building.buildingType = mapTileData[i].tileID;

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
    tile.unitSpriteID = unit.unitType;
    tile.unitIsActive = GameUnit::UnitStateActive;
    mapTileData[unit.mapPosX+unit.mapPosY*mapSize.x] = tile;
  }

  // udpate the player units
  for (uint8_t i = 0; i < aPlayer.units.getCount(); i++) {
    GameUnit unit = aPlayer.units[i];

    // get the corresponding map tile
    MapTile tile = mapTileData[unit.mapPosX+unit.mapPosY*mapSize.x];
    tile.hasUnit = 1;
    tile.unitBelongsTo = (aPlayer == player1)?MapTile::BelongsToPlayer1:MapTile::BelongsToPlayer2;
    tile.unitSpriteID = unit.unitType;
    tile.unitIsActive = unit.activated;
    mapTileData[unit.mapPosX+unit.mapPosY*mapSize.x] = tile;

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
        tile.unitSpriteID = 0;

        // update tile
         mapTileData[y*mapSize.x+x] = tile;
    }
  }
}


void AWGame::markUnitOnMap(const GameUnit *aUnit){
  UnitType unitType = static_cast<UnitType>(aUnit->unitType);
  UnitTraits traits = UnitTraits::traitsForUnitType(unitType);
  uint8_t moveDistance = traits.moveDistance+1;

  // check if maptile is a street and give a small bonus
  MapTile tile = mapTileData[aUnit->mapPosY*mapSize.x+aUnit->mapPosX];
  if (mapTileIsStreet(static_cast<MapTileType>(tile.tileID)))
    moveDistance += 1;

  // hide unit on map since it follow the cursor
  mapTileData[aUnit->mapPosY*mapSize.x+aUnit->mapPosX].hasUnit = 0;

  markPositionAsSelectedForUnit({aUnit->mapPosX, aUnit->mapPosY}, moveDistance, unitType);
}

void AWGame::markPositionAsSelectedForUnit(Point position, int8_t distance, UnitType unit){

  // check if we are at the end
  if (distance <= 0) return;

  // check for bounds
  if (position.x < 0 || position.x >= mapSize.x || position.y < 0 || position.y >= mapSize.y ) return;

  // get the tile
  MapTile tile = mapTileData[position.y*mapSize.x+position.x];

  // check if unit can move to the field
  if (!tile.canBeAccessedByUnit(unit)) return;

  // set maptile to show the selection if there is no other unit
  if (!tile.hasUnit) {
    tile.showSelection = 1;
    mapTileData[position.y*mapSize.x+position.x] = tile;
  }

  // check for movement malus
  if (static_cast<MapTileType>(tile.tileID) == MapTileType::Forest)
    distance--;
  else if (static_cast<MapTileType>(tile.tileID) == MapTileType::Hill)
    distance--;
  else if (static_cast<MapTileType>(tile.tileID) == MapTileType::Mountain)
    distance = min(2, distance);

  // shorten the distance for every step.
  distance--;

  // recursively call this method for every orientation
  markPositionAsSelectedForUnit({position.x+1, position.y}, distance, unit);
  markPositionAsSelectedForUnit({position.x, position.y-1}, distance, unit);
  markPositionAsSelectedForUnit({position.x-1, position.y}, distance, unit);
  markPositionAsSelectedForUnit({position.x, position.y+1}, distance, unit);

  return;
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

void AWGame::markPositionForAttack(Point position, int8_t distance, UnitType unit, AWPlayer &attackingPlayer){

    // check if we are at the end
    if (distance <= 0) return;

    // check player
    uint8_t thisPlayer = (attackingPlayer == player1)?MapTile::BelongsToPlayer1:MapTile::BelongsToPlayer2;

    // check for bounds
    if (position.x < 0 || position.x >= mapSize.x || position.y < 0 || position.y >= mapSize.y ) return;

    // get the tile
    MapTile tile = mapTileData[position.y*mapSize.x+position.x];

    // chek if there is an enemy unit
    if (!tile.showsFog && tile.hasUnit && tile.unitBelongsTo != thisPlayer) {
      tile.showSelection = 1;
      mapTileData[position.y*mapSize.x+position.x] = tile;
    }

    // shorten the distance for every step.
    distance--;

    // recursively call this method for every orientation
    markPositionForAttack({position.x+1, position.y}, distance, unit, attackingPlayer);
    markPositionForAttack({position.x, position.y-1}, distance, unit, attackingPlayer);
    markPositionForAttack({position.x-1, position.y}, distance, unit, attackingPlayer);
    markPositionForAttack({position.x, position.y+1}, distance, unit, attackingPlayer);

    return;
}


Point AWGame::nextMarkedMapPosition(Point currentPosition, int8_t direction){

    // iterate through the map
    for (int8_t i = (currentPosition.x+currentPosition.y*mapSize.x)+direction; i < (mapSize.x*mapSize.y) && i >= 0; i += direction) {

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
      const MapTileType tileType = static_cast<MapTileType>(tile.tileID);

      // draw maptile
      sprites.drawSelfMasked(drawPos.x, drawPos.y, worldSprite, tile.tileID);

      // check if tile below is the HQ
      if (y < mapSize.y-1) {
        const MapTile tileBelow = mapTileData[((y+1)*mapSize.x)+x];
        const MapTileType tileTypeOfTileBelow = static_cast<MapTileType>(tileBelow.tileID);

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
          sprites.drawPlusMask(drawPos.x, drawPos.y, unitSprite, tile.unitSpriteID*2+(arduboy.frameCount/10)%2);
        }
        else{
          // Don't animate if not active and draw a badge to top right.
          // The *2 is because every unit in the spritesheet has two frames
          sprites.drawPlusMask(drawPos.x, drawPos.y, unitSprite, tile.unitSpriteID*2);
          sprites.drawPlusMask(drawPos.x+10, drawPos.y-2, mapMarkers_plus_mask, 2);
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

const GameBuilding * AWGame::getBuildingAtCoordinate(Point coordinate){

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
        if (mapTileIsOpaque(static_cast<MapTileType>(mapTileData[x0+y0*mapSize.x].tileID)))
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
