#ifndef GAME_H
#define GAME_H

#include <Arduboy2.h>
#include <Tinyfont.h>
#include <Sprites.h>

// Documentation
// This files contains our Main game class which defines the base of the game
// and other relevant things.
// In c++ you usually have two files with the same name. One has a '.h' extension
// the other a '.cpp'extension.
// By definition the '.h' file (called Header) contains the definition of things
// and the '.cpp' file contains their implementation. This has historical reasons
// and won't be explained further here.
//
// There are a lot of sources about this on the internet if you re curious.

// Gamestates
// This is an enumaration. It is used to define options for a certain purpose.
// Creating a Enumaration means defining a new type like int or char but only
// for the purpose of storing a option.
//
// E.g. creating a varable with this enum is the same as with int.
// Sample: AWGameState foo;
//  - You can not assign any int value to foo.
//      foo == 8; // -> Wont work.
//  - You can assign an option of AWGameState to foo.
//      foo == AWGameState::showMenu;
//
// Usings enums are not neccesary and their function can be replaced by other
// tricks but it vastly improve readability with some additional type safety so
// it is recommended to use them.
enum class AWGameState{
  showMenu,
  playSinglePlayer,
  playMultiPlayer,
  showOptions
};

// Global defines
// Defines are almost like variables.
// They have no type (like int or char) and cannot be reassigned.
//
// By standard they are written in capital letters so everyone immediately sees
// that they are #defines.
//
// They are mostly used for readability and it is actually not a good practice to use them.
// You can easily replace them with constant variables.
// E.g. const uint8_t scrollspeed_normal = 1;
// But since it is very common to see #defines we stick with it.
#define SCROLLSPEED_NORMAL 1
#define SCROLLSPEED_FAST 3
#define TILESIZE 16


// The Game class
// This is the core of our game.
// It manages the order in which things are happening and stores the Information
// of all neccesary parts of the game.
class AWGame{
  public:
    // This is the constructor.
    // Like setup() it is called once when a new instance of AWGame was created.
    // Start looking at this method in the AWGame.cpp file when done reading this file.
    AWGame();

    // ======================
    // Methods

    // starts the game and calls all methods in neccesary order
    void run(void);

private:
    // ======================
    // Methods

    // Game methods
    AWGameState showMenu();           // Displays the menu
    void startNewSinglePlayerGame();  // Starts a new singleplayer game
    void runSinglePlayerGame();       // Starts a new multiplayer game

    // ======================
    // Data
    // This is our instance of the Arduboy2 class. It is the foundation for
    // every Arduboy project.
    Arduboy2 arduboy;

    // This is our instance of the Sprites class.
    // We need this to draw tilesets and animations which Arduboy2 can not do.
    Sprites sprites;

    // This is tinyfont, a small font we use to print text on the display.
    // It is much smaller that Arduboy2s font so we can fit it better on the screen.
    // The disadvantage is the poor readability.
    Tinyfont tinyfont = Tinyfont(arduboy.getBuffer(), Arduboy2::width(), Arduboy2::height());

    // Here we see our enum again.
    // We create a variable which will store the current gamestate.
    AWGameState gameState;
};
#endif
