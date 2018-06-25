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

bool GameUnit::canUnitAttackUnit(UnitType attacker, UnitType defender){

  // check for none
  if (attacker == UnitType::None || defender == UnitType::None) return false;

  // Read from lookuptable
  uint16_t lutForAttacker = pgm_read_word(attackLookupTable+static_cast<uint16_t>(attacker));

  // get the correct value from lut end return the result
  return (lutForAttacker & (0b1000000000000000 >> static_cast<uint16_t>(defender)) );
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

void AWPlayer::reset(){
  // default 30 money
  money = 30;

  // Set cursor
  cursorIndex = {0,0};

  // clear units
  units.clear();
}

GameUnit * AWPlayer::getUnitForMapCoordinates(Point coordinates){

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


bool MapTile::canBeAccessedByUnit(UnitType unitType){
  // get correct entry from LUT for the unit
  uint8_t lut = 0;
  if (static_cast<int8_t>(unitType) <= static_cast<int8_t>(UnitType::SpecOps))
    lut = 0;
  else if (static_cast<int8_t>(unitType) <= static_cast<int8_t>(UnitType::Missiles))
    lut = 1;
  else if (static_cast<int8_t>(unitType) <= static_cast<int8_t>(UnitType::Bomber))
    lut = 2;
  else if (static_cast<int8_t>(unitType) <= static_cast<int8_t>(UnitType::Transportship))
    lut = 3;
  else return false; // unit is invalid
  lut = pgm_read_byte(moveLookupTable+lut); // reuse the variable

  // get the correct value from lut end return the result
  MapTileType type = static_cast<MapTileType>(this->tileID);
  return (lut >> MapTile::lutIndexForMaptile(type)) & 0x01;
}

// Ground,Water,Street,Hill,Mountain,Forest,Reef,Building
uint8_t MapTile::lutIndexForMaptile(MapTileType mapTileType){
  switch (mapTileType) {

    // Water fields
    case MapTileType::Water:
    case MapTileType::Coast1:
    case MapTileType::Coast12:
    default: return 6;

    // Ground
    case MapTileType::Plains:
    case MapTileType::Grass: return 7;

    // Street
    case MapTileType::Street1:
    case MapTileType::Street2:
    case MapTileType::Street3:
    case MapTileType::Street4:
    case MapTileType::Street5:
    case MapTileType::Street6: return 5;

    // Hill
    case MapTileType::Hill: return 4;

    // Mountain
    case MapTileType::Mountain: return 3;

    // Forest
    case MapTileType::Forest: return 2;

    // Reef
    case MapTileType::Reef: return 1;

    // Building
    case MapTileType::City:
    case MapTileType::Factory:
    case MapTileType::Airport:
    case MapTileType::Shipyard:
    case MapTileType::ScienceLab:
    case MapTileType::P1HQ:
    case MapTileType::P2HQ: return 0;
  }
}
