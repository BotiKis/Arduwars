#include "DataClasses.h"
#include <string.h>

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

GameBuilding::GameBuilding(){
  // Save the provided Type
  buildingType = static_cast<uint8_t>(BuildingType::None);

    // By default set every attribute to 0
  healthPoints = 0;
  movePenalty = 0;
  defenseBonus = 0;
  others = 0;
}

GameBuilding::GameBuilding(BuildingType type){
  // Save the provided Type
  buildingType = static_cast<uint8_t>(type);

  // Every Building has default 10 HP
  healthPoints = 10;

  // set unused stuff to 0
  others = 0;

  // Switch through types
  switch (type) {
    default:
    case BuildingType::Hill:{
      movePenalty   = 1;  // MAX 3
      defenseBonus  = 2;  // MAX 3
      break;
    };
    case BuildingType::Mountain:{
      movePenalty   = 3;  // MAX 3
      defenseBonus  = 2;  // MAX 3
      break;
    };
    case BuildingType::Forest:{
      movePenalty   = 1;  // MAX 3
      defenseBonus  = 1;  // MAX 3
      break;
    };
    case BuildingType::City:{
      movePenalty   = 1;  // MAX 3
      defenseBonus  = 1;  // MAX 3
      break;
    };
    case BuildingType::Factory:{
      movePenalty   = 0;  // MAX 3
      defenseBonus  = 1;  // MAX 3
      break;
    };
    case BuildingType::Airport:{
      movePenalty   = 0;  // MAX 3
      defenseBonus  = 1;  // MAX 3
      break;
    };
    case BuildingType::Shipyard:{
      movePenalty   = 0;  // MAX 3
      defenseBonus  = 1;  // MAX 3
      break;
    };
    case BuildingType::ScienceFacility:{
      movePenalty   = 0;  // MAX 3
      defenseBonus  = 1;  // MAX 3
      break;
    };
    case BuildingType::HQPlayer1:{
      movePenalty   = 0;  // MAX 3
      defenseBonus  = 1;  // MAX 3
      healthPoints  = 15; // HQ is only building with 15 HP
      break;
    };
    case BuildingType::HQPlayer2:{
      movePenalty   = 0;  // MAX 3
      defenseBonus  = 1;  // MAX 3
      healthPoints  = 15; // HQ is only building with 15 HP
      break;
    };
  }
}

// reset the data of this class
void Player::reset(){
  // default 20 money
  money = 20;

  // reset name
  strcpy(name, "\0");

  // Set cursor
  cursorIndex = {0,0};

  // clear units
  units.clear();
}
