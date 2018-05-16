#include "DataClasses.h"
#include <string.h>

// ====================================================
// GameUnit

GameUnit::GameUnit(){
  // Save the provided Type
  unitType = static_cast<uint8_t>(UnitType::Soldier);

  // Every unit has default 10 HP
  healthPoints = 10;

  // set unused stuff to 0
  others = 0;
}

char_P* GameUnit::nameForUnitType(UnitType unitType){
  return LOCA_Unit_Names[static_cast<uint8_t>(unitType)];
}

const UnitTraits UnitTraits::traitsForUnitType(UnitType unitType){
  UnitTraits traits;

  // Switch through types
  switch (unitType) {
    default:
    // Infantry
    case UnitType::Soldier:{
      traits.attackPower  = 1; // MAX 31
      traits.defense      = 0; // MAX 31
      traits.moveDistance = 3; // MAX 7
      traits.attackRange  = 1; // MAX 7
      break;
    };
    case UnitType::Mech:{
      traits.attackPower  = 3; // MAX 31
      traits.defense      = 0; // MAX 31
      traits.moveDistance = 2; // MAX 7
      traits.attackRange  = 1; // MAX 7
      break;
    };
    case UnitType::SpecOps:{
      traits.attackPower  = 3; // MAX 31
      traits.defense      = 0; // MAX 31
      traits.moveDistance = 5; // MAX 7
      traits.attackRange  = 1; // MAX 7
      break;
    };
    // Vehicles
    case UnitType::Recon:{
      traits.attackPower  = 3; // MAX 31
      traits.defense      = 1; // MAX 31
      traits.moveDistance = 6; // MAX 7
      traits.attackRange  = 1; // MAX 7
      break;
    };
    case UnitType::Assist:{
      traits.attackPower  = 0; // MAX 31
      traits.defense      = 1; // MAX 31
      traits.moveDistance = 7; // MAX 7
      traits.attackRange  = 0; // MAX 7
      break;
    };
    case UnitType::Tank:{
      traits.attackPower  = 5; // MAX 31
      traits.defense      = 2; // MAX 31
      traits.moveDistance = 4; // MAX 7
      traits.attackRange  = 1; // MAX 7
      break;
    };
    case UnitType::BigTank:{
      traits.attackPower  = 7; // MAX 31
      traits.defense      = 3; // MAX 31
      traits.moveDistance = 2; // MAX 7
      traits.attackRange  = 1; // MAX 7
      break;
    };
    case UnitType::Artillery:{
      traits.attackPower  = 4; // MAX 31
      traits.defense      = 1; // MAX 31
      traits.moveDistance = 3; // MAX 7
      traits.attackRange  = 5; // MAX 7
      break;
    };
    case UnitType::Rocket:{
      traits.attackPower  = 5; // MAX 31
      traits.defense      = 1; // MAX 31
      traits.moveDistance = 3; // MAX 7
      traits.attackRange  = 4; // MAX 7
      break;
    };
    case UnitType::Missiles:{
      traits.attackPower  = 6; // MAX 7
      traits.defense      = 0; // MAX 3
      traits.moveDistance = 4; // MAX 7
      traits.attackRange  = 6; // MAX 7
      break;
    };
    // Planes
    case UnitType::Heli:{
      traits.attackPower  = 3; // MAX 31
      traits.defense      = 1; // MAX 31
      traits.moveDistance = 5; // MAX 7
      traits.attackRange  = 1; // MAX 7
      break;
    };
    case UnitType::Fighter:{
      traits.attackPower  = 4; // MAX 31
      traits.defense      = 2; // MAX 3
      traits.moveDistance = 7; // MAX 7
      traits.attackRange  = 1; // MAX 7
      break;
    };
    case UnitType::Bomber:{
      traits.attackPower  = 7; // MAX 31
      traits.defense      = 3; // MAX 31
      traits.moveDistance = 3; // MAX 7
      traits.attackRange  = 1; // MAX 7
      break;
    };
    // Ships
    case UnitType::Cruiser:{
      traits.attackPower  = 3; // MAX 31
      traits.defense      = 1; // MAX 31
      traits.moveDistance = 4; // MAX 7
      traits.attackRange  = 1; // MAX 7
      break;
    };
    case UnitType::Battleship:{
      traits.attackPower  = 5; // MAX 31
      traits.defense      = 3; // MAX 31
      traits.moveDistance = 5; // MAX 7
      traits.attackRange  = 4; // MAX 7
      break;
    };
    case UnitType::Transportship:{
      traits.attackPower  = 0; // MAX 31
      traits.defense      = 1; // MAX 31
      traits.moveDistance = 7; // MAX 7
      traits.attackRange  = 0; // MAX 7
      break;
    };
  }

  return traits;
}


// ====================================================
// EnviromentEffects

const EnviromentEffects EnviromentEffects::effectForType(EnviromentType type){
  EnviromentEffects effects;

  // Switch through types
  switch (type) {
    default:
    case EnviromentType::Ground:
    case EnviromentType::Water:{
      effects.moveBonus     = 0;
      effects.defenseBonus  = 0;
      break;
    };
    case EnviromentType::Street:{
      effects.moveBonus     = 1;
      effects.defenseBonus  = 0;
      break;
    };
    case EnviromentType::Hill:{
      effects.moveBonus     = -1;
      effects.defenseBonus  = 2;
      break;
    };
    case EnviromentType::Mountain:{
      effects.moveBonus     = -3;
      effects.defenseBonus  = 2;
      break;
    };
    case EnviromentType::Building: // Building is same as a forest
    case EnviromentType::Forest:{
      effects.moveBonus     = -1;
      effects.defenseBonus  = 1;
      break;
    };
    case EnviromentType::Reef:{
      effects.moveBonus     = -3;
      effects.defenseBonus  = 1;
      break;
    };
  }

  return effects;
}


const bool EnviromentEffects::canEnviromentBeAccessedByUnit(EnviromentType enviromentType, UnitType unitType){

  //Ground,Water,Street,Hill,Mountain,Forest,Reef,Building
  static const uint8_t moveTable[16] =
  {
    0b10111101,  // Soldier
    0b10111101, // Mech
    0b10111101, // SpecOps
    0b10110101, // Recon
    0b10110101, // Assist,
    0b10110101, // Tank
    0b10110101, // BigTank
    0b10110101, // Artillery
    0b10110101, // Rocket,
    0b10110101, // Missiles,
    0b11111111, // Heli,
    0b11111111, // Fighter
    0b11111111, // Bomber,
    0b01000000, // Cruiser,
    0b01000000, // Battleship,
    0b01000000 // Transportship
  };

 uint8_t moveMask = moveTable[static_cast<uint8_t>(unitType)];
 uint8_t value = (moveMask >> (7-static_cast<uint8_t>(enviromentType))) & 0b00000001;

 return (value == 1);
}


// ====================================================
// GameBuilding

GameBuilding::GameBuilding(){
  // By default it's plains
  buildingType = static_cast<uint8_t>(MapTileType::Plains);

  // Every Building has default 10 HP
  healthPoints = 10;

  // Set the coordinates to 0
  mapPosX = mapPosY = 0;
  belongsToPlayer = OwnerShipNone;
}

GameBuilding::GameBuilding(MapTileType type){
  // Save the provided Type
  buildingType = static_cast<uint8_t>(type);

  // Every Building has default 10 HP
  healthPoints = 10;

  // Set the coordinates to 0
  mapPosX = mapPosY = 0;
  belongsToPlayer = OwnerShipNone;
}


// ====================================================
// Player

void Player::reset(){
  // default 20 money
  money = 20;

  // Set cursor
  cursorIndex = {0,0};

  // clear units
  units.clear();
}

// ====================================================
// MapTile

MapTile::MapTile(void){
  tileID = 0;
  buildingBelongsTo = 0;
  unitBelongsTo = OwnerShipNone;
  showSelection = OwnerShipNone;
  showsFog = 0;
  unitSpriteID = 0;
}
