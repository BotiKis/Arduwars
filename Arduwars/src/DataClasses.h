#ifndef DATA_H
#define DATA_H

#include <Arduboy2.h>
#include <List.h>
#include "Localization.h"

// Documentation:
// This File holds all relevant data classes of this game.
// Data Classes are a place where only Information is stored.
// Here will be no visual nor game logic elements of the game.


using GameObjectOwnership = uint8_t;
constexpr static const GameObjectOwnership OwnerShipNone    = 0;
constexpr static const GameObjectOwnership OwnerShipPlayer1 = 1;
constexpr static const GameObjectOwnership OwnerShipPlayer2 = 2;

// This enum defines the possible Units
// There are only 16 in total.
enum class UnitType : int8_t {
  // Infantry
  Soldier = 0,
  Mech,
  SpecOps,

  // Vehicles
  Recon,
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
  Water = 0,
  Reef,
  Coast1,
  // coast stuff from 2-13
  Coast12,
  Plains = 14,
  Grass,
  Street1,
  // street stuff from 16-21
  Street6 = 21,
  Hill = 22,
  Mountain,
  Forest,
  City,
  Factory,
  Airport,
  Shipyard,
  ScienceLab,
  P1HQ,
  P2HQ
};

// Helper function returns true if the given Index is a Building
static inline bool mapTileIndexIsBuilding(MapTileType mapTileidx){
  return (mapTileidx >= MapTileType::City && mapTileidx <= MapTileType::P2HQ);
}

// Helper function returns true if the given Index is a Building with a shop
static inline bool mapTileIndexIsShop(MapTileType mapTileidx){
  return (mapTileidx == MapTileType::Factory || mapTileidx == MapTileType::Airport || mapTileidx == MapTileType::Shipyard);
}

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
  uint8_t activated   :1; // 1 if unit has took its action.
  uint8_t others      :5; // reserved for future use.
  // 3 Bytes in total.

  // Constructor
  // Default constructor initializes an all 0 filled UnitType::Soldier Unit.
  GameUnit();

  // returns the name of a certain UnitType
  static char_P* nameForUnitType(UnitType unitType);

  // returns the costs of a certain UnitType
  static uint8_t costsOfUnit(UnitType unitType);
};

class UnitTraits{
public:
  uint8_t attackPower :5; // Unit have max 31 attackPower.
  uint8_t defense     :5; // Unit have max 31 defense.
  uint8_t moveDistance:3; // Unit can move in a radius of max 7 tiles.
  uint8_t attackRange :3; // Unit can attack in a radius of max 7 tiles.

  // 2 Bytes in total

  // Function returns the traits for a certain unit type
  static const UnitTraits traitsForUnitType(UnitType unitType);
};

// This enum defines the possible Enviroments.
enum class EnviromentType : uint8_t {
  Ground = 0,
  Water,
  Street,
  Hill,
  Mountain,
  Forest,
  Reef,
  Building
};

// Certain enviroments interact with units by giving or taking away certain traits.
class EnviromentEffects{
public:

  int8_t moveBonus     :4; // -4 to 3
  int8_t defenseBonus  :4; // -4 to 3

  // Returns an effect for a given type.
  static const EnviromentEffects effectForType(EnviromentType type);
  static const bool canEnviromentBeAccessedByUnit(EnviromentType enviromentType, UnitType unitType);
};

// This class defines a Building like the HQ or Cities.
// Mountains, Forests and other things on the Map also count as buildings.
class GameBuilding{
public:
  uint8_t buildingType                :5; // We store it as a uint8_t instead of a BuildingType,
                                          // because we save some valuable ram if we use a Bitfield.
                                          // Later we typecast safely between these two types.
  uint8_t healthPoints                :5; // A building can have max 32 health.
  GameObjectOwnership belongsToPlayer :2; // Tells to which player the Building belongs
  uint8_t mapPosX                     :5; // X Position on the map - max 32.
  uint8_t mapPosY                     :5; // Y Position on the map - max 32.
  uint8_t others                      :2; // reserved for future use
  // 3 Bytes in total.

  // Constructor
  // Default constructor initializes an all 0 filled BuildingType::None Building.
  GameBuilding();

  // initializes with corresponding data
  GameBuilding(MapTileType type);
};

// This class defines a Player which has Units, Buildings, Money and other Information.
class Player{
public:

  // Stores the players Money. MAX 255
  // To look more amazing we append two 0s when we are printing it.
  uint8_t money;

  // Remembers the last cursor index of the player
  Point cursorIndex;

  // Stores all the units
  List<GameUnit, 24> units;          // 24 Units make approximately 72 Bytes.
  // Approximately 75 bytes in total.

  // reset the data of this class
  void reset();
};

// This class is used to tell the map drawing method what to draw.
// It is NOT a Datastorage and should only be used to tell the drawing method what to draw.
// An 2D array with instances of this class is populated at the start of every round
// of a player.

class MapTile {
public://MapTileIndex

  MapTile(void);

  uint8_t tileID:5;                        // Holds the ID of the Tile in the Tilesheet
  GameObjectOwnership buildingBelongsTo:2; // Tells to whom the building belongs (if it's a building).
  GameObjectOwnership unitBelongsTo:2;     // Tells, if there is a unit and to which player it belongs. OwnerShipNone if there is no unit
  uint8_t showSelection:1;                 // When 1, it displays the selection Animation
  uint8_t showsFog:1;                      // When 1, it shows fog.
  uint8_t unitSpriteID:5;                  // If field has a Unit, this contains the sprite ID in the Units Spritesheet.

  // 2 bytes in total
};

#endif
