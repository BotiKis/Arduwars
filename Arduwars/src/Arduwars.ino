#include "AWGame.h"

// This is a variable which will store the main game.
// It's not an actual variable to the main game but a pointer to it.
// This means, we don't yet have a main game but we will store it in
// this pointer.
AWGame game = AWGame();

// ---------------------------------------------------
// Setup function
// This is the first code the Arduboy will run.
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
}

// ---------------------------------------------------
// loop function
// As the name suggests, this function is always called again and again until You
// turn of your Arduboy.
void loop() {

  // This starts the actual game by calling this method of the instance of AWGame.
  // Method means the same as function but the name indicates that it is part of
  // a class.
  //
  // Since we have a pointer we can not simply call any method with the '.'
  // operator and we need to use the '->' operator. It is called "pointer-operator".

  game.run();

  // when we call run() it will execute the code there until we return from that function.
  // This loop()-function won't continue until then but that is no problem.
  // From now on we will have to provide our own loops for different things.
  // Loops like that are called game-loops and are part of the core of every game.
  // Don't be overwhelmed with this, it will be explained later.
  //
  // Continue reading with AWGame.h
}
