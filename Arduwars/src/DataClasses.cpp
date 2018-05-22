#include "DataClasses.h"
#include "ShopData.h"

// ====================================================
// GameUnit

GameUnit::GameUnit(){
  // Save the provided Type
  unitType = static_cast<uint8_t>(UnitType::Soldier);

  // Every unit has default 10 HP
  healthPoints = GameUnit::UnitDefaultHealth;

  // set active
  activated = GameUnit::UnitStateActive;

  // set the payload
  payload = GameUnit::PayloadNone;
}

char_P* GameUnit::nameForUnitType(UnitType unitType){
  return LOCA_Unit_Names[static_cast<uint8_t>(unitType)];
}

uint8_t GameUnit::costsOfUnit(UnitType unitType){
  return pgm_read_byte(unitPrices + static_cast<uint8_t>(unitType));
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
    case UnitType::Scout:{
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

  static constexpr uint8_t PROGMEM moveTable[16] =
  {
    //Ground,Water,Street,Hill,Mountain,Forest,Reef,Building
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

 const uint8_t moveMask = pgm_read_byte(moveTable+static_cast<uint8_t>(unitType));
 const uint8_t value = (moveMask >> (7-static_cast<uint8_t>(enviromentType))) & 0b00000001;

 return (value == 1);
}

const EnviromentType EnviromentEffects::enviromentTypeForMapTileType(MapTileType mapTileType){
  switch (mapTileType) {

    // Water fields
    case MapTileType::Water:
    case MapTileType::Coast1:
    case MapTileType::Coast12:
    default: return EnviromentType::Water;

    // Reef
    case MapTileType::Reef: return EnviromentType::Reef;

    // Street
    case MapTileType::Street1:
    case MapTileType::Street2:
    case MapTileType::Street3:
    case MapTileType::Street4:
    case MapTileType::Street5:
    case MapTileType::Street6: return EnviromentType::Street;

    // Hill
    case MapTileType::Hill: return EnviromentType::Hill;

    // Mountain
    case MapTileType::Mountain: return EnviromentType::Mountain;

    // Forest
    case MapTileType::Forest: return EnviromentType::Forest;

    // Ground
    case MapTileType::Plains:
    case MapTileType::Grass: return EnviromentType::Ground;

    // Street
    case MapTileType::City:
    case MapTileType::Factory:
    case MapTileType::Airport:
    case MapTileType::Shipyard:
    case MapTileType::ScienceLab:
    case MapTileType::P1HQ:
    case MapTileType::P2HQ: return EnviromentType::Building;
  }
}

const bool EnviromentEffects::canMapTileTypeBeAccessedByUnit(MapTileType mapTileType, UnitType unitType){
  return EnviromentEffects::canEnviromentBeAccessedByUnit(EnviromentEffects::enviromentTypeForMapTileType(mapTileType),unitType);
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
  isOccupied = 0;
  belongsToPlayer = 0;
}

GameBuilding::GameBuilding(MapTileType type){
  // Save the provided Type
  buildingType = static_cast<uint8_t>(type);

  // Every Building has default 10 HP
  healthPoints = 10;

  // Set the coordinates to 0
  mapPosX = mapPosY = 0;
  isOccupied = 0;
  belongsToPlayer = 0;
}


// ====================================================
// Player

void Player::reset(){
  // default 40 money
  money = 40;

  // Set cursor
  cursorIndex = {0,0};

  // clear units
  units.clear();
}

const GameUnit * Player::unitForMapCoordinates(Point coordinates){

  // go through all units
  for (uint8_t i = 0; i < units.getCount(); i++) {
    // when units is found return it
    if (units[i].mapPosX == coordinates.x && units[i].mapPosY == coordinates.y)
      return &units[i];
  }

  return nullptr;
}

// ====================================================
// MapTile

MapTile::MapTile(void){
  tileID = 0;

  buildingIsOccupied = 0;
  buildingBelongsTo = 0;

  hasUnit = 0;
  unitBelongsTo = 0;

  showSelection = 0;
  showsFog = 0;

  unitIsActive = GameUnit::UnitStateActive;
  unitSpriteID = 0;
}
