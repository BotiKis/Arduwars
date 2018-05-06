#include "AWGame.h"

#include "Tileset.h"
#include "SpriteAnimations.h"
#include "SpriteAssets.h"
#include "MapData.h"

#include "AnimationHelper.h"

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
        case 0: return AWGameState::playSinglePlayer;
        case 1: return AWGameState::playMultiPlayer;
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

void AWGame::startNewSinglePlayerGame(){
  runSinglePlayerGame();
}

void AWGame::runSinglePlayerGame(){
  Point mapOrigin = {0,0};
  Point drawPos = {0,0};

  uint8_t cursorAnimationFrame = 0;
  uint8_t scrollMultiplier = SCROLLSPEED_NORMAL;
  uint32_t cursorAnimationTimestamp = millis();

    // Game loop
    while(true){

        // wait for next frame
        if(!arduboy.nextFrame()) continue;

        // Handle Button Inputs
        arduboy.pollButtons();

        if (arduboy.pressed(DOWN_BUTTON)){
          mapOrigin.y -= scrollMultiplier;
        }
        if (arduboy.pressed(UP_BUTTON)){
          mapOrigin.y += scrollMultiplier;
        }
        if (arduboy.pressed(LEFT_BUTTON)){
          mapOrigin.x += scrollMultiplier;
        }
        if (arduboy.pressed(RIGHT_BUTTON)){
          mapOrigin.x -= scrollMultiplier;
        }
        if (arduboy.pressed(A_BUTTON)){
          scrollMultiplier = SCROLLSPEED_FAST;
        }
        else{
          scrollMultiplier = SCROLLSPEED_NORMAL;
        }

        // animate the cursor
        if (MILLIS_SINCE(cursorAnimationTimestamp) > 500) {
          cursorAnimationFrame = (1+cursorAnimationFrame)%2;
          cursorAnimationTimestamp = millis();
        }

        // Draw
        arduboy.clear();

        // draw the map
        for (size_t y = 0; y < 16; y++) {
          for (size_t x = 0; x < 24; x++) {

            drawPos.x = mapOrigin.x+x*16;
            drawPos.y = mapOrigin.y+y*16;

            // ignore if out of bounds
            if (drawPos.x <= -16 || drawPos.x >= 128 || drawPos.y <= -16 || drawPos.y >= 80) continue;

            uint8_t spriteIDX = pgm_read_byte(mapData24x16+y*24+x);

            if (spriteIDX == 30) {
              sprites.drawOverwrite(drawPos.x, drawPos.y-16, worldSprite, 32);
            }
            if (spriteIDX == 31) {
              sprites.drawOverwrite(drawPos.x, drawPos.y-16, worldSprite, 33);
            }
            sprites.drawSelfMasked(drawPos.x, drawPos.y, worldSprite, spriteIDX);
          }
        }

        // draw cursorIdx
        sprites.drawPlusMask(48, 16, gameCursorAnimation_plus_mask, cursorAnimationFrame);

        arduboy.display();

    }
}
