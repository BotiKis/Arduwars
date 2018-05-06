#ifndef DATA_H
#define DATA_H

#include <stdint.h>
#include <Array.h>

// Documentation:
// This File holds all relevant data classes of this game.
// Data Classes are a place where only Information is stored.
// There will be no visual nor game logic elements of the game.

// This enum defines the possible Units
// There are only 16 in total.
enum class UnitType : uint8_t {
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
  Transportship // Can transport Infantry
};

// This enum defines the possible buildings.
// A Building can be the HQ, a city or even a Mountain or Forests.
// There are only 16 in total.
enum class BuildingType : uint8_t {
  None = 0,
  Hill,     // Can only be moved to by Infantry
  Mountain, // Can only be moved to by Infantry
  Forest,
  City,
  Factory,
  Airport,
  Shipyard,
  ScienceFacility,
  HQPlayer1,
  HQPlayer2
};


// This class defines a Unit like Soldiers or Tanks
class Unit{
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
  // So by doing this we use 24 Bits in total which only need 3 Bytes instead
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
  uint8_t attackPower :3; // Can have max 7 attackPower.
  uint8_t defense     :2; // Can have max 3 defense.
  uint8_t moveDistance:3; // Can move in a radius of max 7 tiles.
  uint8_t attackRange :3; // Can attack in a radius of max 7 tiles.
  uint8_t others      :5; // reserved for future use.
  // 3 Bytes in total.

  // Constructor
  // Default constructor initializes an all 0 filled UnitType::Soldier Unit.
  Unit();
  // Custom Constructor is used to initialize with default content. See DataClass.cpp for the implementation.
  Unit(UnitType atype);
};

// This class defines a Building like the HQ or Cities.
// Mountains, Forests and other things on the Map also count as buildings.
class Building{
public:
  uint8_t buildingType  :4; // We can have 16 different buildings
                            // Also we store it as a uint8_t instead of a BuildingType,
                            // because we save some valuable ram if we use a Bitfield.
                            // Later we typecast safely between these two types.
  uint8_t healthPoints  :4; // A building can have max 15 health.
  uint8_t movePenalty   :2; // A building can give a moving Unit a movement penalty up to 3.
  uint8_t defenseBonus  :2; // A building can give a stationed Unit a defense bonus up to 3.
  uint8_t others        :4; // reserved for future use.
  // 2 Bytes in total.

  // Constructor
  // Default constructor initializes an all 0 filled BuildingType::None Building.
  Building();
  // Custom Constructor is used to initialize with default content. See DataClass.cpp for the implementation.
  Building(BuildingType type);
};

// This class defines a Player which has Units, Buildings, Money and other Information.
class Player{
public:
  uint8_t money;
  Array<Unit, 24> units;          // 24 Units make approximately 72 Bytes.
  Array<Building, 16> buildings;  // 16 Buildings approximately makes 32 Bytes.
  // Approximately 106 bytes in total.

  // Constructor is used to initialize with default content. See DataClass.cpp for the implementation.
  Player();
};

#endif
