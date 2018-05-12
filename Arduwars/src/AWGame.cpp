#include "AWGame.h"

#include "Tileset.h"
#include "SpriteAnimations.h"
#include "SpriteAssets.h"
#include "MapData.h"

#include <UtilityFunctions.h>
#include "PointMath.h"

#include <MemoryFree.h>

// -------------------------------------------------------
// Constructor sets up basic stuff for the game
AWGame::AWGame(){

  // First we need to initialize our Arduboy instance by called the usual methods.
  arduboy.boot(); //arduboy.begin(); <- I will forget this.
  #warning switch to .begin()
  arduboy.setFrameRate(60);
  arduboy.initRandomSeed();
  arduboy.audio.on();

  // Set up text
  tinyfont.setTextColor(BLACK);

  // Initialize players
  player1 = new Player();
  strcpy(player1->name, "PLAYER 1");

  player2 = new Player();
  strcpy(player2->name, "PLAYER 2");

  // Now we set our Gamestate to showMenu since we want to start there.
  gameState = AWGameState::showMenu;
}

// This method starts the actual game and is called in the Arduwars.ino file.
void AWGame::run(void){

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
          this->gameState = AWGameState::showMenu;
          break;
        }
        case AWGameState::playMultiPlayer:{
          // start game
          startNewMultiplayerPlayerGame();
          this->gameState = AWGameState::showMenu;
          break;
        }
        case AWGameState::showOptions:{
          this->gameState = AWGameState::showMenu;
          break;
        }
        case AWGameState::showMenu:
        default:
          // When showMenu is the current state, we will call the showMenu() method.
          // Take a look at the method now!
          this->gameState = showMenu();
      }
  }
}


// This method displays the Main menu to the player.
AWGameState AWGame::showMenu(){

  // In this variable we will store the index of the cursor so
  // we know what the player has selected.
  int8_t cursorIdx = 0;

  // Again a Game loop
  while(true){

    // wait for next frame since we don't want to do things more than that.
    // There may be isntances where we would do stuff between frames (e.g. smmulations, calculations, etc.)
    // but for this project we stick with this.
    if(!arduboy.nextFrame()) continue;

    // Now we handle the button input of the arduboy
    arduboy.pollButtons();

    if (arduboy.justPressed(DOWN_BUTTON)){
      cursorIdx++;
    }
    if (arduboy.justPressed(UP_BUTTON)){
      cursorIdx--;
    }
    if (arduboy.justPressed(B_BUTTON)){

      // We do this here for random numbers we need later.
      // Random numbers are a complicated topic and won't be explained further here.
      arduboy.initRandomSeed();

      // React to users choice
      switch (cursorIdx) {
        case 0:{
          return showMapSelection(AWGameState::playSinglePlayer);
        }
        case 1: {
          return showMapSelection(AWGameState::playMultiPlayer);
        }
        case 2: return AWGameState::showOptions;
        default: return AWGameState::showMenu; // this default is not needed but it's safe to do this.
      }
    }

    // Here we limit and wrap the cursor so it cannot be larger than 2 nor smaller then 0.
    cursorIdx = (cursorIdx<0)?2:cursorIdx;
    cursorIdx = cursorIdx%3;

    // Now we start drawing the menu to the screen

    // This clears the dispaly and makes everything black.
    arduboy.clear();

    // fill the screen white
    arduboy.fillScreen(WHITE);

    // Here we draw the menu text
    // The text is wrapped in a F() function which tells the compiler to
    // Store the text in PROGMEM instead of RAM which saves us some valuable RAM.
    tinyfont.setCursor(2, 7);
    tinyfont.print(F("SINGLE PLAYER"));
    tinyfont.setCursor(2, 13);
    tinyfont.print(F("TWO PLAYER"));
    tinyfont.setCursor(2, 19);
    tinyfont.print(F("OPTIONS"));

    // depending on the cursor index, we draw the cursor
    tinyfont.setCursor(75, 7 + cursorIdx*6);
    tinyfont.print("<");

    // print free memory
    printFreeMemory();

    // this one draws everything we have drawn to the actual screen.
    arduboy.display();

    // Now this loop wraps around and keeps repeating until the user selects
    // an menu item.
  }
}

// This method displays the map selection menu to the player.
AWGameState AWGame::showMapSelection(AWGameState nextState){
  // In this variable we will store the index of the cursor so
  // we know what the player has selected.
  int8_t cursorIdx = 0;
  unsigned const char *mapData;

  // Again a Game loop
  while(true){

    // Skip the frame
    if(!arduboy.nextFrame()) continue;

    // Now we handle the button input of the arduboy
    arduboy.pollButtons();

    if (arduboy.justPressed(DOWN_BUTTON)){
      cursorIdx++;
    }
    if (arduboy.justPressed(UP_BUTTON)){
      cursorIdx--;
    }
    if (arduboy.justPressed(B_BUTTON)){

      // select map to users choice
      switch (cursorIdx) {
        case 0:{
          mapData = mapData_3;
          break;
        }
        case 1:{
          mapData = mapData_2;
          break;
        }
        case 2:{
          mapData = mapData_1;
          break;
        }
        default: return AWGameState::showMenu;
      }

      // clear old map Data
      if (mapTileData != nullptr) {
        delete [] mapTileData;
        mapTileData = nullptr;
      }

      // handle map data
      mapSize.x  = pgm_read_byte(mapData);
      mapSize.y  = pgm_read_byte(mapData+1);
      mapSizeInPixel = mapSize*TILE_SIZE;

      //// Load map data
      // Calculate map lenght
      uint16_t mapLenght = mapSize.x*mapSize.y;

      // create new array of maptiles
      mapTileData = new MapTile[mapLenght];

      // Populate array from map data
      for (uint16_t i = 0; i < mapLenght; i++) {
        mapTileData[i].tileID = pgm_read_byte(mapData+2+i);
      }

      return nextState;
    }
    if (arduboy.justPressed(A_BUTTON)) {
      // When the A Button is pressed we go back to the menu.
      return AWGameState::showMenu;
    }

    // Limit and wrap the index
    cursorIdx = (cursorIdx<0)?2:cursorIdx;
    cursorIdx = cursorIdx%3;

    // This clears the dispaly and makes everything black.
    arduboy.clear();

    // fill the screen white
    arduboy.fillScreen(WHITE);

    // Draw the menu
    tinyfont.setCursor(2, 8);
    tinyfont.print(F("< BACK WITH A"));

    tinyfont.setCursor(2, 17);
    tinyfont.print(F("SKIRMISH"));
    tinyfont.setCursor(2, 23);
    tinyfont.print(F("BATTLE"));
    tinyfont.setCursor(2, 29);
    tinyfont.print(F("ARDUWARS"));

    // depending on the cursor index, we draw the cursor
    tinyfont.setCursor(64, 17 + cursorIdx*6);
    tinyfont.print("<");

    // print free memory
    printFreeMemory();

    // this one draws everything we have drawn to the actual screen.
    arduboy.display();
  }
}

void AWGame::startNewSinglePlayerGame(){
  // reset day count;
  daysPlayed = 0;

  // reset players
  player1->reset();
  player2->reset();

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

    arduboy.clear();

    // fill the screen white
    arduboy.fillScreen(WHITE);

    tinyfont.setCursor(1,1);
    tinyfont.print("Singleplayer...");

    arduboy.display();
  }
}

void AWGame::startNewMultiplayerPlayerGame(){
  // reset day count;
  daysPlayed = 0;

  // reset players
  player1->reset();
  player2->reset();

  runMultiPlayerGame();
}

void AWGame::runMultiPlayerGame(){

  Player *currentPlayer = player1;

  // Game Loops
  while (true) {

    // show player round
    doRoundOfPlayer(currentPlayer);

    // check for win condition
    if (daysPlayed == 255) {
      #warning implement win condition
      return;
    }

    // switch players
    if (currentPlayer == player1)
      currentPlayer = player2;
    else{
      currentPlayer = player1;
      daysPlayed++;
    }

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
    drawHudForPlayer(currentPlayer);

    arduboy.display();

    showDialog(currentPlayer->name);
  }
}

void AWGame::doRoundOfPlayer(Player *currentPlayer){

  uint8_t scrollMultiplier = SCROLLSPEED_NORMAL;

  // Store cursor data
  Point cursorPosition = currentPlayer->cursorIndex*TILE_SIZE;
  cursorPosition.x -= 8;
  cursorPosition.y -= 8;
  Point cameraPosition = {0, 0};
  Point currentIndex = {0, 0};

    // Game loop
    while(true){

        // wait for next frame
        if(!arduboy.nextFrame()) continue;

        // Handle Button Inputs
        arduboy.pollButtons();

        if (arduboy.pressed(DOWN_BUTTON)){
          cursorPosition.y += scrollMultiplier;
        }
        if (arduboy.pressed(UP_BUTTON)){
          cursorPosition.y -= scrollMultiplier;
        }
        if (arduboy.pressed(LEFT_BUTTON)){
          cursorPosition.x -= scrollMultiplier;
        }
        if (arduboy.pressed(RIGHT_BUTTON)){
          cursorPosition.x += scrollMultiplier;
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
        sprites.drawPlusMask(cursorPosition.x-cameraPosition.x, cursorPosition.y-cameraPosition.y+1, gameCursorAnimation_plus_mask, (arduboy.frameCount/30)%2); // the +1 looks more correct

        // Draw HUD
        drawHudForPlayer(currentPlayer);

        // log index
        // arduboy.fillRect(0, 64-12, 11, 12, WHITE);
        // tinyfont.setCursor(1, 64 - 11);
        // tinyfont.print(currentIndex.x);
        // tinyfont.setCursor(1, 64 - 5);
        // tinyfont.print(currentIndex.y);

        // print free memory
        printFreeMemory();

        arduboy.display();

        // Check if menu was pressed
        if (arduboy.justPressed(B_BUTTON)) {
          if(showOption("END TURN")){
            currentPlayer->cursorIndex = currentIndex;
            return;
          }
        }
    }
}

void AWGame::drawMapAtPosition(Point pos){
  Point drawPos;

  // draw the map
  for (int8_t y = 0; y < mapSize.y; y++) {
    for (int8_t x = 0; x < mapSize.x; x++) {

      drawPos.x = pos.x+x*TILE_SIZE;
      drawPos.y = pos.y+y*TILE_SIZE;

      // ignore if out of bounds
      if (drawPos.x <= -TILE_SIZE || drawPos.x >= arduboy.width() || drawPos.y <= -TILE_SIZE || drawPos.y >= (arduboy.height()+TILE_SIZE)) continue;

      // get the tile
      MapTile tile = mapTileData[y*mapSize.x+x];

      if (tile.tileID == 30) {
        sprites.drawOverwrite(drawPos.x, drawPos.y-TILE_SIZE, worldSprite, 32);
      }
      if (tile.tileID == 31) {
        sprites.drawOverwrite(drawPos.x, drawPos.y-TILE_SIZE, worldSprite, 33);
      }
      sprites.drawSelfMasked(drawPos.x, drawPos.y, worldSprite, tile.tileID);
    }
  }
}

void AWGame::showDialog(const char *titleText){

  // frame for the dialog
  static Rect frame;

  frame.width = strlen(titleText)*5+8;
  frame.height = 16;
  frame.x = (arduboy.width() - frame.width)/2;
  frame.y = 24;

  Point textPos;
  textPos.x = (arduboy.width() - strlen(titleText)*5)/2;
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
    arduboy.fillRoundRect(frame.x-1, frame.y-1, frame.width+2, frame.height+2, 5, WHITE);
    arduboy.fillRoundRect(frame.x, frame.y, frame.width, frame.height, 5, BLACK);
    arduboy.fillRoundRect(frame.x + 1, frame.y + 1, frame.width-2, frame.height-3, 5, WHITE);

    // OK Button
    tinyfont.setCursor(textPos.x, textPos.y);
    tinyfont.print(titleText);

    arduboy.display();
  }
}

bool AWGame::showOption(const char *buttonTitle){

  // frame for the dialog
  static Rect frame;

  frame.width = strlen(buttonTitle)*5+8;
  frame.height = 14;
  frame.x = arduboy.width() - frame.width - 4;
  frame.y = 10;

  // dialog loop
  while (true) {

    // wait for next frame
    if(!arduboy.nextFrame()) continue;

    // Get input
    arduboy.pollButtons();

    // Exit on button press
    if (arduboy.justPressed(A_BUTTON)){
      return false;
    }  // Exit on button press
    if (arduboy.justPressed(B_BUTTON)){
      return true;
    }

    // Drawing
    // Infobox
    arduboy.fillRoundRect(frame.x-1, frame.y-1, frame.width+2, frame.height+2, 5, WHITE);
    arduboy.fillRoundRect(frame.x, frame.y, frame.width, frame.height, 5, BLACK);
    arduboy.fillRoundRect(frame.x + 1, frame.y + 1, frame.width-2, frame.height-3, 5, WHITE);

    // OK Button
    tinyfont.setCursor(frame.x + 4, frame.y + 5);
    tinyfont.print(buttonTitle);

    arduboy.display();
  }
}

void AWGame::drawHudForPlayer(Player *aPlayer){
  // Draw player, day and funds
  arduboy.fillRect(0, 0, 128, 7, BLACK);
  arduboy.fillRect(0, 0, 128, 6, WHITE);
  tinyfont.setCursor(1, 1);
  tinyfont.print(aPlayer->name);

  tinyfont.setCursor(46, 1);
  tinyfont.print(F("DAY:"));
  tinyfont.setCursor(66, 1);
  tinyfont.print(daysPlayed);
  tinyfont.setCursor(86, 1);
  tinyfont.print(F("$:"));
  tinyfont.setCursor(100, 1);
  tinyfont.print(aPlayer->money*100);
}

Point AWGame::calculateCameraPosition(Point forCursorPosition){
  Point cameraPosition;

  // Calculate camera offset
  cameraPosition.x = forCursorPosition.x - (arduboy.width()-32)/2;
  cameraPosition.y = forCursorPosition.y - (arduboy.height()-32)/2;

  // Check for bounds
  if(cameraPosition.x < 0) cameraPosition.x = 0;
  if(cameraPosition.y < 1) cameraPosition.y = 1;
  if(cameraPosition.x > mapSizeInPixel.x-arduboy.width()) cameraPosition.x = mapSizeInPixel.x-arduboy.width();
  if(cameraPosition.y > mapSizeInPixel.y-arduboy.height()+mapOffsetY) cameraPosition.y = mapSizeInPixel.y-arduboy.height()+mapOffsetY;

  return cameraPosition;
}

void AWGame::printFreeMemory(){

  arduboy.fillRect(0, arduboy.height()-7, arduboy.width(), 6, BLACK);
  arduboy.fillRect(0, arduboy.height()-6, arduboy.width(), 6, WHITE);

  tinyfont.setCursor(1, arduboy.height()-5);
  tinyfont.print(F("MEM FREE:"));
  tinyfont.setCursor(48, arduboy.height()-5);
  tinyfont.print(freeMemory());
}
