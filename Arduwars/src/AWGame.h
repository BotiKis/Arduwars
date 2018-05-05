#ifndef GAME_H
#define GAME_H

#include <Arduboy2.h>
#include <Tinyfont.h>
#include <Sprites.h>

// Gamestates
enum class AWGameState{
  showMenu,
  playSinglePlayer,
  playMultiPlayer,
  showOptions
};

// Global defines
#define SCROLLSPEED_NORMAL 1
#define SCROLLSPEED_FAST 3
#define TILESIZE 16


// Game class
class AWGame{
  public:
    AWGame();

    // ======================
    // Methods

    /// starts the game and calls all methods in neccesary order
    void run(void);

private:
    // ======================
    // Methods

    // Game methods
    AWGameState showMenu();
    void startNewSinglePlayerGame();
    void runSinglePlayerGame();

    // ======================
    // Data
    Arduboy2 arduboy;
    Sprites sprites;
    Tinyfont tinyfont = Tinyfont(arduboy.getBuffer(), Arduboy2::width(), Arduboy2::height());
    AWGameState gameState;
};
#endif
