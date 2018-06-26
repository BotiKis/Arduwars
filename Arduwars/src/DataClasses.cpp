#include "DataClasses.h"
#include "ShopData.h"
#include "ProgmemReadAnything.h"

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

  pgm_readAnything(&allUnitTraits[static_cast<uint8_t>(unitType)], traits);

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
  const uint8_t movement = pgm_read_byte(moveLookupTable+lut); // reuse the variable

  // get the correct value from lut end return the result
  MapTileType type = static_cast<MapTileType>(this->tileID);
  return (movement & MapTile::lutMaskForMaptile(type)) != 0;
}

// Ground,Water,Street,Hill,Mountain,Forest,Reef,Building
uint8_t MapTile::lutMaskForMaptile(MapTileType mapTileType){
  switch (mapTileType) {

    // Water fields
    case MapTileType::Water:
    case MapTileType::Coast1:
    case MapTileType::Coast12:
    default: return (1 << 6);

    // Ground
    case MapTileType::Plains:
    case MapTileType::Grass: return (1 << 7);

    // Street
    case MapTileType::Street1:
    case MapTileType::Street2:
    case MapTileType::Street3:
    case MapTileType::Street4:
    case MapTileType::Street5:
    case MapTileType::Street6: return (1 << 5);

    // Hill
    case MapTileType::Hill: return (1 << 4);

    // Mountain
    case MapTileType::Mountain: return (1 << 3);

    // Forest
    case MapTileType::Forest: return (1 << 2);

    // Reef
    case MapTileType::Reef: return (1 << 1);

    // Building
    case MapTileType::City:
    case MapTileType::Factory:
    case MapTileType::Airport:
    case MapTileType::Shipyard:
    case MapTileType::ScienceLab:
    case MapTileType::P1HQ:
    case MapTileType::P2HQ: return (1 << 0);
  }
}
