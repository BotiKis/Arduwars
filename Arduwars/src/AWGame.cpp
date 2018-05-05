#include "AWGame.h"

#include "Tileset.h"
#include "SpriteAnimations.h"
#include "SpriteAssets.h"
#include "MapData.h"

#include "AnimationHelper.h"

// -------------------------------------------------------
// Constructor sets up basic stuff for the game

AWGame::AWGame(){
  arduboy.boot();
  arduboy.setFrameRate(60);
  arduboy.initRandomSeed();
  arduboy.audio.on();
  //arduboy.invert(true);
  gameState = AWGameState::showMenu;
}

void AWGame::run(void){
  // Game loop
  while(true){

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
          this->gameState = showMenu();
      }
  }
}


AWGameState AWGame::showMenu(){
  uint8_t cursorIdx = 0;

  // Game loop
  while(true){
    arduboy.pollButtons();

    if (arduboy.justPressed(DOWN_BUTTON)){
      cursorIdx++;
    }
    if (arduboy.justPressed(UP_BUTTON)){
      cursorIdx--;
    }
    if (arduboy.justPressed(B_BUTTON)){
      arduboy.initRandomSeed();

      // Single player
      if (cursorIdx == 0)
        return AWGameState::playSinglePlayer;
      if (cursorIdx == 1)
        return AWGameState::playMultiPlayer;
      if (cursorIdx == 2)
        return AWGameState::showOptions;

    }
    cursorIdx = cursorIdx%3;

    arduboy.clear();

    tinyfont.setCursor(2, 47);
    tinyfont.print(F("SINGLE PLAYER"));
    tinyfont.setCursor(2, 53);
    tinyfont.print(F("TWO PLAYER"));
    tinyfont.setCursor(2, 59);
    tinyfont.print(F("OPTIONS"));
    tinyfont.setCursor(75, 47 + cursorIdx*6);
    tinyfont.print("<");

    arduboy.display();
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

            uint8_t spriteIDX = pgm_read_byte(mapData20x10+y*24+x);

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
