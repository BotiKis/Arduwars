#include "AWGame.h"

#include "Tileset.h"
#include "SpriteAnimations.h"
#include "SpriteAssets.h"
#include "MapData.h"

#include <UtilityFunctions.h>
#include "PointMath.h"

// -------------------------------------------------------
// Constructor sets up basic stuff for the game
AWGame::AWGame(){

  // First we need to initialize our Arduboy instance by called the usual methods.
  arduboy.boot(); //arduboy.begin(); <- I will forget this.
  arduboy.setFrameRate(60);
  arduboy.initRandomSeed();
  arduboy.audio.on();

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

    // Here we draw the menu text
    // The text is wrapped in a F() function which tells the compiler to
    // Store the text in PROGMEM instead of RAM which saves us some valuable RAM.
    tinyfont.setCursor(2, 47);
    tinyfont.print(F("SINGLE PLAYER"));
    tinyfont.setCursor(2, 53);
    tinyfont.print(F("TWO PLAYER"));
    tinyfont.setCursor(2, 59);
    tinyfont.print(F("OPTIONS"));

    // depending on the cursor index, we draw the cursor
    tinyfont.setCursor(75, 47 + cursorIdx*6);
    tinyfont.print("<");

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
          mapData = mapData12x12;
          break;
        }
        case 1:{
          mapData = mapData20x12;
          break;
        }
        case 2:{
          mapData = mapData24x16;
          break;
        }
        default: return AWGameState::showMenu;
      }

      // handle map data
      mapSize.x  = pgm_read_byte(mapData);
      mapSize.y  = pgm_read_byte(mapData+1);
      mapSizeInPixel = mapSize*TILE_SIZE;

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

    // Draw the menu
    tinyfont.setCursor(2, 38);
    tinyfont.print(F("< BACK WITH A"));

    tinyfont.setCursor(2, 47);
    tinyfont.print(F("SKIRMISH"));
    tinyfont.setCursor(2, 53);
    tinyfont.print(F("SEABATTLE"));
    tinyfont.setCursor(2, 59);
    tinyfont.print(F("WAR PLANES"));

    // depending on the cursor index, we draw the cursor
    tinyfont.setCursor(64, 47 + cursorIdx*6);
    tinyfont.print("<");

    // this one draws everything we have drawn to the actual screen.
    arduboy.display();
  }
}

void AWGame::startNewSinglePlayerGame(){
  runSinglePlayerGame();
}

void AWGame::runSinglePlayerGame(){

  uint8_t scrollMultiplier = SCROLLSPEED_NORMAL;

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

        // Calculate camera offset
        cameraPosition.x = cursorPosition.x - (arduboy.width()-32)/2;
        cameraPosition.y = cursorPosition.y - (arduboy.height()-32)/2;

        // Check for bounds
        if(cameraPosition.x < 0) cameraPosition.x = 0;
        if(cameraPosition.y < 0) cameraPosition.y = 0;
        if(cameraPosition.x > mapSizeInPixel.x-arduboy.width()) cameraPosition.x = mapSizeInPixel.x-arduboy.width();
        if(cameraPosition.y > mapSizeInPixel.y-arduboy.height()) cameraPosition.y = mapSizeInPixel.y-arduboy.height();

        // calc currentIndex
        currentIndex.x = ((int16_t)(cursorPosition.x+TILE_SIZE))/TILE_SIZE;
        currentIndex.y = ((int16_t)(cursorPosition.y+TILE_SIZE))/TILE_SIZE;

        // Start Drawing
        arduboy.clear();

        // Draw the map first
        drawMapAtPosition(cameraPosition * -1);

        // Draw the cursor on top
        sprites.drawPlusMask(cursorPosition.x-cameraPosition.x, cursorPosition.y-cameraPosition.y, gameCursorAnimation_plus_mask, (arduboy.frameCount/30)%2);

        // log index
        arduboy.fillRect(0, 64-12, 11, 12, BLACK);
        tinyfont.setCursor(1, 64 - 11);
        tinyfont.print(currentIndex.x);
        tinyfont.setCursor(1, 64 - 5);
        tinyfont.print(currentIndex.y);

        arduboy.display();
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

      // +2 because the first two bytes are the width and height.
      uint8_t spriteIDX = pgm_read_byte(mapData+2+y*mapSize.x+x);

      if (spriteIDX == 30) {
        sprites.drawOverwrite(drawPos.x, drawPos.y-TILE_SIZE, worldSprite, 32);
      }
      if (spriteIDX == 31) {
        sprites.drawOverwrite(drawPos.x, drawPos.y-TILE_SIZE, worldSprite, 33);
      }
      sprites.drawSelfMasked(drawPos.x, drawPos.y, worldSprite, spriteIDX);
    }
  }
}
