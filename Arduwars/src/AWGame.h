#ifndef GAME_H
#define GAME_H

#include <Arduboy2.h>
#include <Tinyfont.h>
#include <Sprites.h>
#include <List.h>

#include "DataClasses.h"
#include "Localization.h"

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
// for the purpose of storing an option.
//
// E.g. creating a varable with this enum is the same as with int.
// Sample: AWGameState foo;
//
// But this enum is scoped. That means you cannot assign anything
// but an AWGameState option to it.
// Sample:
//  - You can not assign any int value to foo.
//      foo == 8; // -> Wont work.
//  - You can assign an option of AWGameState to foo.
//      foo == AWGameState::showMenu;
//
// Usings enums are not neccesary and their function can be replaced by other
// tricks but it vastly improve readability with some additional type safety so
// it is recommended to use them.
enum class AWGameState : uint8_t {
  showMainMenu,
  showMapSelection,
  playSinglePlayer,
  playMultiPlayer,
  toggleSound
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
#define TILE_SIZE 16


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
    AWGameState showMainMenu();  // Displays the menu

    // Displays map selection
    // It returns a pointer to the selected map in progmem. nullptr if none was selected
    // Attribute is the current state, only multi/singleplayer is relevant.
    unsigned const char * showMapSelection(AWGameState aState);

    void startNewSinglePlayerGame();  // Starts a new singleplayer game
    void runSinglePlayerGame();       // Place where the singleplayer game runs

    void startNewMultiplayerPlayerGame();  // Starts a new multiplayer game
    void runMultiPlayerGame();       // Place where the multiplayer game runs

    void doRoundOfPlayer(Player *currentPlayer);

    // Game Helper
    void drawMapAtPosition(Point pos); // Draws the map at the given position

    // Shows a dialog with a text at the center of the Screen.
    // closes if B is pressed
    void showDialog(char_P *titleText);

    // Shows a option dialog with a text at the top right.
    // Used as contextmenu in-game
    // Returns false of B is pressed, true if A is pressed
    bool showOption(char_P *buttonTitle);

    // shows the shop UI for the given Building.
    // Will isntantly return UnitType::None if the building
    // is not the Factory, Airport or the Shipyard.
    // Also returns UnitType::None if the PLayer doesn't buy anything.
    UnitType showShopForBuildingAndPlayer(MapTileType building, Player *aPlayer);

    // Shows a HUD with name, Days and Funds
    void drawHudForPlayer(Player *aPlayer);

    // Helper to calculate the camera
    Point calculateCameraPosition(Point forCursorPosition);

    // Loads the map from progmem to memory
    void loadMap(unsigned const char *mapData);

    // updates the gamemap for the player
    void updateMapForPlayer(Player *aPlayer);

    // removes all units from the map
    // withFog == true fills the gamemap with fog
    void clearMap(bool withFog);

    // Marks the given unit on the map by showing it's movement radius
    void markUnitOnMap(const GameUnit *aUnit);

    // clears the units marker
    void unmarkUnitOnMap();

    // Mark the map at the given position and radius regarding visibility rules
    // e.g. Not through buildings and mountains
    void markMapAtPositionAndRadius(Point origin, uint8_t radius, bool removeFog, bool showSelection);

    // neat effect directly on the display buffer
    void makeScreenTransition();

    // Helper method to get a Pointer to the Gamebuilding for a certain location on the map
    // The parameter position is in map coordinates.
    // returns nullptr if there is no building at the Location
    const GameBuilding * getBuildingAtCoordinate(Point coordinate);

    // print free memory // debug Only
    void printFreeMemory();

    // ======================
    // Data
    // This is our instance of the Arduboy2Base class. It is the foundation for
    // every Arduboy project. We use Arduboy2Base instead of Arduboy2 since we
    // wont use the print functions.
    Arduboy2Base arduboy;

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

    // In these two variables we store the players.
    // There are always two players where the first one is the actual player and the second the AI.
    Player *player1;
    Player *player2;

    // This attribute stores the days.
    // A day passes if both players has ended their rounds.
    // The game ends with a draw when it reaches 255 days.
    uint8_t daysPlayed;

    // In this attribute we store the buildings on the map - max 16.
    List<GameBuilding, 24> gameBuildings;

    // Map data
    MapTile *mapTileData = nullptr;
    Point mapSize = {0, 0}; // Mapsize is in Maptile coordinates and not in screen coordinates.
    static constexpr uint8_t mapOffsetY = 8;
};
#endif
