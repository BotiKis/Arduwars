#include "AWGame.h"

// Main game
AWGame *game;

// ---------------------------------------------------
// Setup func
void setup() {
  // put your setup code here, to run once:
  game = new AWGame();
}

// ---------------------------------------------------
// loop func
void loop() {
  game->run();
}
