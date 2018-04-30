#include "AWGame.h"
// -------------------------------------------------------
// Constructor sets up basic stuff for the game

AWGame::AWGame(){
  arduboy.boot();
  arduboy.setFrameRate(60);
  arduboy.initRandomSeed();
  arduboy.audio.on();
  arduboy.invert(true);
  gameState = AWGameState::Menu;
}

void AWGame::run(void){

}
