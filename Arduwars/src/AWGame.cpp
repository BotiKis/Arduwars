#include "AWGame.h"
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
  arduboy.clear();
  tinyfont.setCursor(0, 0);
  tinyfont.print("Single player");
  arduboy.display();
  delay(2000);
}

void AWGame::runSinglePlayerGame(){

}
