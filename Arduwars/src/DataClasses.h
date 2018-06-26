#ifndef DATA_H
#define DATA_H

#include <Arduboy2.h>
#include <List.h>
#include "Localization.h"

// Documentation:
// This File holds all relevant data classes of this game.
// Data Classes are a place where only Information is stored.
// Here will be no visual nor game logic elements of the game.

// This enum defines the possible Units
// There are only 16 in total.
enum class UnitType : int8_t {
  // Infantry
  Soldier = 0,
  Mech,
  SpecOps,

  // Vehicles
  Scout,
  Assist,      // Heals all adjacent Units at the start of the turn and can carry Infantry
  Tank,
  BigTank,
  Artillery,
  Rocket,       // Can only shoot Planes and Ships
  Missiles,

  // Planes
  Heli,
  Fighter,      // Can Only fight Planes
  Bomber,

  // Ships    // Can only go over water
  Cruiser,    // Can only fight Ships
  Battleship,
  Transportship, // Can transport Infantry

  // Needed for the shopsystem
  None = -1
};

// This enum defines the possible Map Tiles
// There are only 32 in total.
enum class MapTileType:uint8_t{
  Water       = 0,
  Reef        = 1,
  Coast1      = 2,
  // coast stuff from 2-13
  Coast12     = 13,
  Plains      = 14,
  Grass       = 15,
  Street1     = 16,
  Street2     = 17,
  Street3     = 18,
  Street4     = 19,
  Street5     = 20,
  Street6     = 21,
  Hill        = 22,
  Mountain    = 23,
  Forest      = 24,
  City        = 25,
  Factory     = 26,
  Airport     = 27,
  Shipyard    = 28,
  ScienceLab  = 29,
  P1HQ        = 30,
  P2HQ        = 31
};

// Helper function returns true if the given Index is a Building
static inline bool mapTileIndexIsBuilding(MapTileType mapTileidx){
  return (mapTileidx >= MapTileType::City && mapTileidx <= MapTileType::P2HQ);
}

// Helper function returns true if the given Index is a Building with a shop
static inline bool mapTileIndexIsShop(MapTileType mapTileidx){
  return (mapTileidx == MapTileType::Factory || mapTileidx == MapTileType::Airport || mapTileidx == MapTileType::Shipyard);
}

// Helper function returns false if a unit can see throug the tile at the given Index
static inline bool mapTileIsOpaque(MapTileType mapTileidx){
  return (mapTileidx == MapTileType::Hill || mapTileidx == MapTileType::Mountain);
}

// Helper function returns true if the maptile is a street
static inline bool mapTileIsStreet(MapTileType mapTileidx){
  return (mapTileidx >= MapTileType::Street1 && mapTileidx <= MapTileType::Street6);
}



// ====================================================
// GameUnit

// This is a Loockuptable that tells which unit can attack which unit.
constexpr uint16_t PROGMEM attackLookupTable[16] =
{
  // Infantry
  0b1111111111000000,  // Soldier
  0b1111111111000000, // Mech
  0b1111111111000000, // SpecOps

  // Vehicles
  0b1111111111000000, // Scout
  0b0000000000000000, // Assist
  0b1111111111000000, // Tank
  0b1111111111000000, // BigTank
  0b1111111111111111, // Artillery
  0b0000000000111111, // Rocket
  0b1111111111111111, // Missiles

  // Planes
  0b1111111111111111, // Heli
  0b0000000000111000, // Fighter
  0b1111111111111111, // Bomber

  // Ships
  0b0000000000000111, // Cruiser
  0b1111111111111111, // Battleship
  0b0000000000000000, // Transportship
};

// This class defines a Unit like Soldiers or Tanks
class GameUnit{
public:
  // For Beginners:
  // By appending a ':NUMBER' to an attribute of an class you create a Bitfield.
  // Usually a uint8_t takes up 8 Bits of memory which is 1 Byte.
  // So an object of this class would take up 8 Bytes.
  //
  // But we are very memory limited so we try to save as much as possible.
  // By creating a Bitfield we share single bits of a byte.
  //
  // E.g. uniteType and healthPoints would take up 2 Byte in total but by
  // appending ':4' we tell the compiler to only use 4 bits of the Byte.
  // And since the compiler is smart enough it will combine those bits into
  // one single byte.
  //
  // So by doing this we use 19 Bits in total which only need 3 Bytes instead
  // of the original 8.
  //
  // Note:
  // Bitfields are some kind of magic and their implementation is dependent on
  // different factors. That means that they may work different on other
  // architectures/platforms than the Arduboy.

  uint8_t unitType    :4; // We can have 16 different Units going from 0-15
                          // Also we store it as a uint8_t instead of a UnitType,
                          // because we save some valuable ram if we use a Bitfield.
                          // Later we typecast safely between these two types.
  uint8_t healthPoints:4; // A unit can have max 15 health.
  uint8_t mapPosX     :5; // X Position on the map - max 32.
  uint8_t mapPosY     :5; // Y Position on the map - max 32.
  uint8_t activated   :1; // GameUnit::UnitStateDisabled if unit has took its action. Default... GameUnit::UnitStateActive

  // If it's a transport unit we store here the index of the unit in
  // the players Units which is transported.
  // Default... GameUnit::PayloadNone if there is no unit.
  // It has the value of 31 which is save, since every player can only have 24 units.
  uint8_t payload     :5;

  // 3 Bytes in total.
  // Every Player can have 24 Units. That makes 144 Bytes in RAM.

  // Constructor
  // Default constructor initializes an all 0 filled UnitType::Soldier Unit.
  GameUnit();

  // returns the name of a certain UnitType
  static char_P* nameForUnitType(UnitType unitType);

  // returns the costs of a certain UnitType
  static uint8_t costsOfUnit(UnitType unitType);

  // Helper for the payload attribute
  // since a player can have max 24 Units we can use every value above as a helper.
  // we use 31 to indicate, that there is no payload.
  static constexpr uint8_t PayloadNone = 31;

  // Helper for units active state
  static constexpr uint8_t UnitStateActive    = 1;
  static constexpr uint8_t UnitStateDisabled  = 0;

  // Default health for units
  static constexpr uint8_t UnitDefaultHealth  = 10;

  // Static method tells if a unit can attack another one.
  static bool canUnitAttackUnit(UnitType attacker, UnitType defender);
};

class UnitTraits{
public:
  uint8_t moveDistance:3; // Unit can move in a radius of max 7 tiles.
  uint8_t attackPower :5; // Unit have max 31 attackPower.
  uint8_t attackRange :3; // Unit can attack in a radius of max 7 tiles.
  uint8_t defense     :5; // Unit have max 31 defense.
  // 2 Bytes in total.

  // Function returns the traits for a certain unit type
  static const UnitTraits traitsForUnitType(UnitType unitType);
};

constexpr UnitTraits PROGMEM allUnitTraits[16] = {
  // Infantry
  {3,1,1,0},  // Soldier
  {2,3,1,0}, // Mech
  {5,3,1,0}, // SpecOps

  // Vehicles
  {6,3,1,1}, // Scout
  {7,0,0,1}, // Assist
  {4,5,1,2}, // Tank
  {2,7,1,3}, // BigTank
  {3,4,5,1}, // Artillery
  {3,5,4,1}, // Rocket
  {4,6,6,0}, // Missiles

  // Planes
  {5,3,1,1}, // Heli
  {7,4,1,2}, // Fighter
  {3,7,1,3}, // Bomber

  // Ships
  {4,3,1,1}, // Cruiser
  {5,5,4,3}, // Battleship
  {7,0,0,1}, // Transportship
};

// ====================================================
// GameBuilding

// This class defines a Building like the HQ or Cities.
// Mountains, Forests and other things on the Map also count as buildings.
class GameBuilding{
public:
  uint8_t buildingType    :5; // We store it as a uint8_t instead of a BuildingType,
                              // because we save some valuable ram if we use a Bitfield.
                              // Later we typecast safely between these two types.
  uint8_t healthPoints    :5; // A building can have max 32 health.
  uint8_t isOccupied      :1; // If the Building is occupied or not
  uint8_t belongsToPlayer :1; // Tells to which player the Building belongs
  uint8_t mapPosX         :5; // X Position on the map - max 32.
  uint8_t mapPosY         :5; // Y Position on the map - max 32.
  uint8_t others          :2; // reserved for future use
  // 3 Bytes in total.

  // Constructor
  // Default constructor initializes an all 0 filled BuildingType::None Building.
  GameBuilding();

  // initializes with corresponding data
  GameBuilding(MapTileType type);

  // attribute is the global viewdistance for all buildings
  static constexpr uint8_t buildingViewDistance = 3;
};


// ====================================================
// Player

// This class defines a Player which has Units, Buildings, Money and other Information.
class AWPlayer{
public:

  // Stores the players Money. MAX 255
  // To look more amazing we append two 0s when we are printing it.
  uint8_t money;

  // Remembers the last cursor index of the player
  Point cursorIndex;

  // Stores all the units
  List<GameUnit, 24> units; // 24 Units make approximately 72 Bytes.
  // Approximately 75 bytes in total.

  // reset the data of this class
  void reset();

  // return a unit for give coordinates on the map.
  // nullptr if no unit found.
  GameUnit * getUnitForMapCoordinates(Point coordinates);

  // Default income for one building
  static constexpr uint8_t BaseIncome = 10;
};

// This class is used to tell the map drawing method what to draw.
// It is NOT a Datastorage except for maptiles and should only be used to tell
// the drawing method what to draw. An 2D array with instances of this class
// is populated at the start of every round of a game.

// ====================================================
// MapTile

constexpr uint8_t PROGMEM moveLookupTable[4] =
{
  // Ground,Water,Street,Hill,Mountain,Forest,Reef,Building
  0b10111101, // Infantry
  0b10110101, // Ground
  0b11111111, // Air
  0b01000000, // Water
};

class MapTile {
public:

  // Helper constants
  static constexpr uint8_t BelongsToPlayer1 = 0;
  static constexpr uint8_t BelongsToPlayer2 = 1;
  static constexpr uint8_t BelongsToPlayer  = 0;
  static constexpr uint8_t BelongsToEnemy   = 1;

  // constructor
  MapTile(void);

  // members
  uint8_t tileID:5;              // Holds the ID of the Tile in the Tilesheet
  uint8_t buildingIsOccupied:1;  // 0 if building is not occupied
  uint8_t buildingBelongsTo:1;   // Tells to whom the building belongs (if it's a building).
  uint8_t hasUnit:1;             // 0... No unit, 1... Contains a unit
  uint8_t unitBelongsTo:1;       // Tells, if there is a unit and to which player it belongs.
  uint8_t showSelection:1;       // When 1, it displays the selection Animation
  uint8_t showsFog:1;            // When 1, it shows fog.
  uint8_t unitIsActive:1;        // Default GameUnit::UnitStateActive.
  uint8_t unitSpriteID:4;        // If field has a Unit, this contains the sprite ID in the Units Spritesheet.
  // 2 bytes in total
  // with a 24x16 array this makes 768 Bytes in RAM. This is about 50% of the available RAM.

  // Returns true if the provided unit can enter the maptile
  bool canBeAccessedByUnit(UnitType unitType);

private:
  // returns a number from 0-7 at the position of the type in the LUT
  static uint8_t lutMaskForMaptile(MapTileType mapTileType);
};

#endif
