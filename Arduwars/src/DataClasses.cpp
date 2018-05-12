#include "DataClasses.h"
#include <string.h>

// ====================================================
// GameUnit

GameUnit::GameUnit(){
  // Save the provided Type
  unitType = static_cast<uint8_t>(UnitType::Soldier);

  // By default set every attribute to 0
  healthPoints = 0;
  attackPower = 0;
  moveDistance = 0;
  attackRange = 0;
}

GameUnit::GameUnit(UnitType type){
  // Save the provided Type
  unitType = static_cast<uint8_t>(type);

  // Every unit has default 10 HP
  healthPoints = 10;

  // set unused stuff to 0
  others = 0;

  // Switch through types
  switch (type) {
    default:
    // Infantry
    case UnitType::Soldier:{
      attackPower  = 1; // MAX 7
      defense      = 0; // MAX 3
      moveDistance = 3; // MAX 7
      attackRange  = 1; // MAX 7
      break;
    };
    case UnitType::Mech:{
      attackPower  = 3; // MAX 7
      defense      = 0; // MAX 3
      moveDistance = 2; // MAX 7
      attackRange  = 1; // MAX 7
      break;
    };
    case UnitType::SpecOps:{
      attackPower  = 3; // MAX 7
      defense      = 0; // MAX 3
      moveDistance = 5; // MAX 7
      attackRange  = 1; // MAX 7
      break;
    };
    // Vehicles
    case UnitType::Recon:{
      attackPower  = 3; // MAX 7
      defense      = 1; // MAX 3
      moveDistance = 6; // MAX 7
      attackRange  = 1; // MAX 7
      break;
    };
    case UnitType::Assist:{
      attackPower  = 0; // MAX 7
      defense      = 1; // MAX 3
      moveDistance = 7; // MAX 7
      attackRange  = 0; // MAX 7
      break;
    };
    case UnitType::Tank:{
      attackPower  = 5; // MAX 7
      defense      = 2; // MAX 3
      moveDistance = 4; // MAX 7
      attackRange  = 1; // MAX 7
      break;
    };
    case UnitType::BigTank:{
      attackPower  = 7; // MAX 7
      defense      = 3; // MAX 3
      moveDistance = 2; // MAX 7
      attackRange  = 1; // MAX 7
      break;
    };
    case UnitType::Artillery:{
      attackPower  = 4; // MAX 7
      defense      = 1; // MAX 3
      moveDistance = 3; // MAX 7
      attackRange  = 5; // MAX 7
      break;
    };
    case UnitType::Rocket:{
      attackPower  = 5; // MAX 7
      defense      = 1; // MAX 3
      moveDistance = 3; // MAX 7
      attackRange  = 4; // MAX 7
      break;
    };
    case UnitType::Missiles:{
      attackPower  = 6; // MAX 7
      defense      = 0; // MAX 3
      moveDistance = 4; // MAX 7
      attackRange  = 6; // MAX 7
      break;
    };
    // Planes
    case UnitType::Heli:{
      attackPower  = 3; // MAX 7
      defense      = 1; // MAX 3
      moveDistance = 5; // MAX 7
      attackRange  = 1; // MAX 7
      break;
    };
    case UnitType::Fighter:{
      attackPower  = 4; // MAX 7
      defense      = 2; // MAX 3
      moveDistance = 7; // MAX 7
      attackRange  = 1; // MAX 7
      break;
    };
    case UnitType::Bomber:{
      attackPower  = 7; // MAX 7
      defense      = 3; // MAX 3
      moveDistance = 3; // MAX 7
      attackRange  = 1; // MAX 7
      break;
    };
    // Ships
    case UnitType::Cruiser:{
      attackPower  = 3; // MAX 7
      defense      = 1; // MAX 3
      moveDistance = 4; // MAX 7
      attackRange  = 1; // MAX 7
      break;
    };
    case UnitType::Battleship:{
      attackPower  = 5; // MAX 7
      defense      = 3; // MAX 3
      moveDistance = 5; // MAX 7
      attackRange  = 4; // MAX 7
      break;
    };
    case UnitType::Transportship:{
      attackPower  = 0; // MAX 7
      defense      = 1; // MAX 3
      moveDistance = 7; // MAX 7
      attackRange  = 0; // MAX 7
      break;
    };

  }
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
  // Save the provided Type
  buildingType = static_cast<uint8_t>(BuildingType::None);

  // By default set every attribute to 0
  healthPoints = 0;
  mapPosX = mapPosY = 0;
}

GameBuilding::GameBuilding(BuildingType type){
  // Save the provided Type
  buildingType = static_cast<uint8_t>(type);

  // Every Building has default 5 HP
  healthPoints = 5;
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
  unitBelongsTo = mapTileNone;
  showSelection = mapTileNone;
  showsFog = 0;
  others = 0;
}
