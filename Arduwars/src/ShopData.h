#ifndef SHOP_H
#define SHOP_H
#include <avr/pgmspace.h>
#include "DataClasses.h"

// This file contains all data for the shop system
// We store the data in progmem.

// Here we store the prices of every unit buyable at a building.
// The order of the prices is the same as of UnitType
const uint8_t unitPrices[] PROGMEM = {
  10,
  30,
  50,

  40,
  50,
  70,
  160,
  60,
  150,
  120,

  90,
  200,
  220,

  180,
  255,
  120
};

// All units obtainable at a factory
constexpr static uint8_t NumberOfBuyableUnitsAtFactory = 10;
const UnitType buyableUnitsAtFactory[NumberOfBuyableUnitsAtFactory] PROGMEM = {
  UnitType::Soldier,
  UnitType::Mech,
  UnitType::SpecOps,

  // Vehicles
  UnitType::Scout,
  UnitType::Assist,
  UnitType::Tank,
  UnitType::BigTank,
  UnitType::Artillery,
  UnitType::Rocket,
  UnitType::Missiles
};

// All units obtainable at an airport
constexpr static uint8_t NumberOfBuyableUnitsAtAirport = 3;
const UnitType buyableUnitsAtAirPort[] PROGMEM = {
  UnitType::Heli,
  UnitType::Fighter,
  UnitType::Bomber,
};

// All units obtainable at a shipyard
constexpr static uint8_t NumberOfBuyableUnitsAtShipyard = 3;
const UnitType buyableUnitsAtShipyard[] PROGMEM = {
  UnitType::Transportship,
  UnitType::Cruiser,
  UnitType::Battleship
};

#endif
