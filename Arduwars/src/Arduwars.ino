#include "AWGame.h"

// This object will hold the main game
AWGame *game;

// ---------------------------------------------------
// Setup func
void setup() {

  // For beginers:
  // 'new' is a special operator which is used to create an instance of a class.
  // It returns a pointer of this instance which is stored in the 'game' variable.
  // Creating a class this way is called dynamic allocation.
  //
  // Usualy every object (that's what an instance of a class is called) has
  // to be deleted when it's no longer used to free up valuable memory.
  //
  // But there is o need to delete this since it only needs to be deleted when
  // the game is turned off and that is only when the Arduboy is turned off.
  // And when the Arduboy is turned off, it does not need to free any memory. :D

  game = new AWGame();
}

// ---------------------------------------------------
// loop func
void loop() {
  // Starts the game
  game->run();
}
