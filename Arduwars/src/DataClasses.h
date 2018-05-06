#ifndef DATA_H
#define DATA_H

#include <stdint.h>
#include <Array.h>

// Documentation:
// This File holds all relevant data classes of this game.
// Data Classes are a place where only Information is stored.
// There will be no visual nor game logic elements of the game.


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
  uint8_t healthPoints:4; // A unit can have max 15 health.
  uint8_t attackPower :2; // Can have max 3 attackPower.
  uint8_t defense     :2; // Can have max 3 defense.
  uint8_t moveDistance:3; // Can move in a radius of max 7 tiles.
  uint8_t attackRange :3; // Can attack in a radius of max 7 tiles.
  uint8_t others      :6; // reserved for future use.
  // 3 Bytes in total.
};

// This class defines a Building like the HQ or Cities.
// Mountains, Forests and other things on the Map also count as buildings.
class Building{
  uint8_t buildingType  :4; // We can have 16 different buildings
  uint8_t healthPoints  :4; // A building can have max 15 health.
  uint8_t movePenalty   :2; // A building can give a moving Unit a movement penalty up to 3.
  uint8_t defenseBonus  :2; // A building can give a stationed Unit a defense bonus up to 3.
  uint8_t others        :4; // reserved for future use.
  // 2 Bytes in total.
};

// This class defines a Player which has Units, Buildings, Money and other Information.
class Player{
  uint8_t money;
  Array<Unit, 24> units;          // 24 Units make 72 Bytes + some overhead.
  Array<Building, 16> buildings;  // 16 Buildings makes 32 Bytes + some overhead.
  // Approximately 110 bytes.
};

#endif
