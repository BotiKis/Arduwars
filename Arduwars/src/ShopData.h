#pragma once

#include <avr/pgmspace.h>
#include "DataClasses.h"
#include "ArrayUtils.h"

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
const UnitType buyableUnitsAtFactory[] PROGMEM = {
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
constexpr static uint8_t NumberOfBuyableUnitsAtFactory = arrayLength(buyableUnitsAtFactory);

// All units obtainable at an airport
const UnitType buyableUnitsAtAirPort[] PROGMEM = {
  UnitType::Heli,
  UnitType::Fighter,
  UnitType::Bomber,
};
constexpr static uint8_t NumberOfBuyableUnitsAtAirport = arrayLength(buyableUnitsAtAirPort);

// All units obtainable at a shipyard
const UnitType buyableUnitsAtShipyard[] PROGMEM = {
  UnitType::Transportship,
  UnitType::Cruiser,
  UnitType::Battleship
};
constexpr static uint8_t NumberOfBuyableUnitsAtShipyard = arrayLength(buyableUnitsAtShipyard);
