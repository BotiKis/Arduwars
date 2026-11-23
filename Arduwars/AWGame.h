#ifndef GAME_H
#define GAME_H

#include <Arduboy2.h>
#include <Tinyfont.h>
#include <Sprites.h>
#include "List.h"

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

// These states define the possible states during the turn of a player.
enum class AWTurnState : uint8_t {
  Default,
  UnitSelected,
  UnitWalking,  // the unit walks along the arrow, or back
  UnitMoved,    // the unit walked, the menu asks what it does
  UnitAttack
};

// Global variables
constexpr uint8_t SCROLLSPEED_NORMAL = 1;
constexpr uint8_t SCROLLSPEED_FAST = 3;
constexpr uint8_t TILE_SIZE = 16;


// The Game class
// This is the core of our game.
// It manages the order in which things are happening and stores the Information
// of all neccesary parts of the game.
class AWGame{
  public:
    // This is the constructor.
    // Like setup() it is called once when a new instance of AWGame was created.
    // Start looking at this method in the AWGame.cpp file when done reading this file.
    AWGame(void) = default;

    // ======================
    // Methods

    // starts the game and calls all methods in neccesary order
    void run(void);

private:
    // ======================
    // Methods

    // Game methods
    AWGameState showMainMenu();  // Displays the menu

    // Title screen menu shared by the main menu and the map selection.
    // Returns the selected index, -1 if cancelled (only with a subtitle).
    int8_t showTitleMenu(char_P * const items[], uint8_t count, char_P *subtitle, int8_t cursorIdx);

    // Displays map selection
    // It returns a pointer to the selected map in progmem. nullptr if none was selected
    // Attribute is the current state, only multi/singleplayer is relevant.
    const unsigned char * showMapSelection(AWGameState aState);

    // Starts a new single player (against the AI) or multiplayer game
    void startNewGame(AWGameState mode);

    // Place where a game runs. With againstAI the AI plays player 2.
    void runGame(bool againstAI);

    // One turn of a player. With aiTurn the AI plays it and the screen
    // shows player 1's view.
    void doRoundOfPlayer(AWPlayer &currentPlayer, bool aiTurn);

    // ======================
    // Enemy AI, in AWGameAI.cpp

    // Picks where the unit goes and builds the path there. Returns the tile
    // and puts the enemy it attacks there in target, {-1,-1} for none.
    Point aiPlanMove(GameUnit *unit, AWPlayer &ai, Point &target);

    // The unit has arrived: it attacks target or captures, then waits.
    // After an ambush it only attacks what it sees now.
    __attribute__((noinline)) void aiAct(GameUnit *unit, Point target, AWPlayer &ai, bool ambushed);

    // Buys new units at every free factory, airport and shipyard of the AI.
    void aiBuyUnits(AWPlayer &ai);

    // Light randomness: the next number of a fixed shuffled order (0-255).
    uint8_t nextRandom();
    uint8_t randomState = 0;

    // ======================

    // Adds a new unit with full health that can act from the next turn on.
    void addNewUnit(AWPlayer &owner, UnitType unitType, Point position);

    // Units sold at a building. Returns the count, 0 if it is not a shop.
    static uint8_t shopListForBuilding(MapTileType building, const UnitType *&buyableUnits);

    // Game Helper
    void drawMapAtPosition(Point pos); // Draws the map at the given position

    // Shows a dialog with a text at the center of the Screen.
    // closes if B is pressed
    void showDialog(char_P *titleText);

    // Shows a option dialog with the titles at the top right of the screen.
    // Used as contextmenu in-game
    // Returns the index of the selected option.
    // Returns -1 if cancelled with A_BUTTON.
    int8_t showOptions(char_P *options[], uint8_t count);

    // Shared drawing and input for dialogs and option lists
    int8_t showBox(char_P * const options[], uint8_t count, bool isDialog);

    // shows the shop UI for the given Building.
    // Will isntantly return UnitType::None if the building
    // is not the Factory, Airport or the Shipyard.
    // Also returns UnitType::None if the PLayer doesn't buy anything.
    UnitType showShopForBuildingAndPlayer(MapTileType building, AWPlayer &aPlayer);

    // Shows a HUD with name, Days and Funds
    void drawHudForPlayer(AWPlayer &aPlayer);

    // Helper to calculate the camera
    __attribute__((noinline)) Point calculateCameraPosition(Point forCursorPosition);

    // Moves the camera a step towards target: halfway, then it locks on.
    __attribute__((noinline)) void glideCameraTo(Point target);

    // Loads the map from progmem to memory
    void loadMap(const unsigned char *mapData);

    // updates the gamemap for the player
    void updateMapForPlayer(AWPlayer &aPlayer);

    // removes all units from the map
    // withFog == true fills the gamemap with fog
    void clearMap(bool withFog);

    // Marks the given unit on the map by showing it's movement radius
    void markUnitOnMap(const GameUnit *aUnit);

    // clears the units marker
    void unmarkUnitOnMap(const GameUnit *aUnit);

    // Movement range flood fill. Returns the scratch array (in the screen
    // buffer) with the most movement left on entering each tile, 0 = unreachable.
    uint8_t * floodMovement(const GameUnit *aUnit);

    // Flood fill from any tile with the given movement into remaining.
    // Visible units of the side that is not moving block the way.
    uint8_t * floodFrom(Point start, UnitType unitType, int8_t moves, uint8_t *remaining);

    // Movement left after leaving a tile of the given type
    static int8_t movementLeftAfter(MapTileType type, int8_t left);

    // Finds a shortest path for the unit to target and stores it in pathDirections.
    void buildPathTo(const GameUnit *aUnit, Point target);

    // Ambush: if the path runs into a hidden enemy, cuts it to end on the
    // last free tile before it and returns true.
    bool cutPathAtAmbush(const GameUnit *aUnit);

    // How many units of the side that is not moving are outside the fog.
    uint8_t countVisibleEnemies();

    // Draws the movement arrow along pathDirections.
    void drawPath(const GameUnit *aUnit, Point camera);

    // Draws a unit sprite of player 1 or 2. Animated units bob.
    void drawUnit(int16_t x, int16_t y, bool firstPlayer, uint8_t unitType, bool animate);


    // Looks at every visible enemy the unit could attack from position.
    // Returns the best damage (0 = nothing in range) and that enemy's tile in target.
    // With markTargets, the targets are marked on the map for the player.
    uint8_t scanTargets(const GameUnit *unit, Point position, AWPlayer &attackingPlayer, Point &target, bool markTargets);

    // returns the position of the next marked tile.
    // Arguments:
    // optional currentPosition... the point from where it searches. Default is 0,0.
    // optional direction... in which direction it will be searched. Default is 1. 1 goes forward, -1 backward
    // Returns {-1,-1} if no marker has been found.
    Point nextMarkedMapPosition(Point currentPosition = {0,0}, int8_t direction = 1);

    // Mark the map at the given position and radius regarding visibility rules
    // e.g. Not through buildings, mountains and enemy units
    void removeFogAtPositionRadiusAndPlayer(Point origin, uint8_t radius, AWPlayer &aPlayer, bool seeThrough = false);

    // Helper function for removeFogAtPositionRadiusAndPlayer
    void castRayTo(Point origin, bool seeThrough, AWPlayer &aPlayer, int8_t xEnd, int8_t yEnd);

    // Wipe between turns: out to black, or (wipeIn) in from black to the buffer's frame
    void makeScreenTransition(bool wipeIn);

    // Helper method to get a Pointer to the Gamebuilding for a certain location on the map
    // The parameter position is in map coordinates.
    // returns nullptr if there is no building at the Location
    GameBuilding * getBuildingAtCoordinate(Point coordinate);

    // Resolves an attack of the given unit on the enemy unit at target,
    // including the counterattack. Removes destroyed units and sets winner.
    void attackUnitAtPosition(GameUnit *attacker, Point target, AWPlayer &attackingPlayer);

    // About one second of animation: the cursor blinks on first and second,
    // the unit on first flickers, or it hops. The camera glides there first.
    // Shows nothing and returns false when both tiles are in the viewer's fog.
    static constexpr uint8_t EffectBlink = 0;   // battle
    static constexpr uint8_t EffectFlicker = 1; // destroyed unit
    static constexpr uint8_t EffectHop = 2;     // capture
    bool playEffect(Point first, Point second, uint8_t effect);

    // Inverts the screen briefly the given number of times (a hit).
    void flashScreen(uint8_t times);

    // Infantry on a building lowers its capture points; takes it at 0.
    // Capturing the enemy HQ sets winner.
    void captureBuildingWithUnit(const GameUnit *unit, AWPlayer &capturingPlayer);

    // Resets capture progress on buildings nobody is standing on anymore.
    void resetAbandonedCaptures();

    // Returns true if the given unit could start or continue capturing at position.
    bool canUnitCaptureAtPosition(const GameUnit *unit, Point position, AWPlayer &aPlayer);

    // Returns the BelongsToPlayer1/2 value for a player.
    uint8_t playerIndex(AWPlayer &aPlayer){ return (aPlayer == player1) ? MapTile::BelongsToPlayer1 : MapTile::BelongsToPlayer2; }

    // ======================
    // Data
    // This is our instance of the Arduboy2Base class. It is the foundation for
    // every Arduboy project. We use Arduboy2Base instead of Arduboy2 since we
    // wont use the print functions.
    Arduboy2Base arduboy;

    // This is our instance of the Sprites class.
    // We need this to draw tilesets and animations which Arduboy2 can not do.
    SpritesB sprites;

    // This is tinyfont, a small font we use to print text on the display.
    // It is much smaller that Arduboy2s font so we can fit it better on the screen.
    // The disadvantage is the poor readability.
    Tinyfont tinyfont = Tinyfont(arduboy.getBuffer(), Arduboy2::width(), Arduboy2::height());

    // Here we see our enum again.
    // We create a variable which will store the current gamestate.
	// We also give it a default value using a C++11 technique.
    AWGameState gameState = AWGameState::showMainMenu;

    // In these two variables we store the players.
    // In single player the first one is the actual player and the second the AI.
    AWPlayer player1 = AWPlayer(1);
    AWPlayer player2 = AWPlayer(2);

    // This attribute stores the days.
    // A day passes if both players has ended their rounds.
    // The game ends with a draw when it reaches 255 days.
    uint8_t daysPlayed;

    // Where the screen looks at the map, in map pixels.
    Point cameraPosition = {0, 0};

    // Whose view the screen shows: the current player, or player 1 against the AI.
    AWPlayer *viewingPlayer = &player1;

    // Whose turn it is (BelongsToPlayer1/2). Their units block nobody of
    // their own side.
    uint8_t movingSide = MapTile::BelongsToPlayer1;

    // Movement path (0 right, 1 up, 2 left, 3 down) from the selected unit to the cursor
    // The longest move is 8 steps: 7 tiles plus 1 when starting on a street.
    static constexpr uint8_t MaxPathLength = 8;
    uint8_t pathDirections[MaxPathLength];
    uint8_t pathLength = 0;

    // The last walk ran into a hidden enemy and stopped before it.
    // showAmbush stays true until the AMBUSH message was shown.
    // (Members, not locals of the turn loop: that costs less flash.)
    bool ambushed = false;
    bool showAmbush = false;

    // Enemies in sight before the walk. If it uncovers more, or after an
    // ambush, the menu offers no Cancel.
    uint8_t enemiesSeen = 0;
    bool canCancel = true;

    // Set when a game is decided. NoWinner while the game is running.
    static constexpr uint8_t NoWinner = 0xFF;
    uint8_t winner = NoWinner; // BelongsToPlayer1 or BelongsToPlayer2 when decided

    // In this attribute we store the buildings on the map - max 16.
    List<GameBuilding, 24> gameBuildings;

    // Map data
    // The largest map is 24x16. The state is kept in a fixed array so its RAM
    // use shows up in the compiler's size report (no runtime allocation).
    static constexpr uint8_t MaxMapWidth = 24;
    static constexpr uint8_t MaxMapHeight = 16;
    MapTile mapTileData[MaxMapWidth*MaxMapHeight];
    const unsigned char *mapData = nullptr; // terrain of the current map in PROGMEM
    static constexpr uint8_t MapDataHeaderSize = 10; // same as MAPDATAOFFSET_Main in MapData.h
    Point mapSize = {0, 0}; // Mapsize is in Maptile coordinates and not in screen coordinates.

    // Terrain lookup straight from PROGMEM
    __attribute__((noinline)) MapTileType tileTypeAtIndex(uint16_t index);
    __attribute__((noinline)) MapTileType tileTypeAt(int16_t x, int16_t y);

    // Cursor pixel position for a map tile
    __attribute__((noinline)) static Point cursorForTile(Point tile);

    // Prints a number with tinyfont at the current cursor
    void printNumber(uint16_t number);
    static constexpr uint8_t mapOffsetY = 8;
};
#endif
