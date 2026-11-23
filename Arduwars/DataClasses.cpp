#include "DataClasses.h"
#include "ShopData.h"
#include "ProgmemReadAnything.h"

// ====================================================
// GameUnit

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

uint8_t GameUnit::calculateDamage(UnitType attacker, uint8_t attackerHealth, UnitType defender, MapTileType defenderTile, bool defenderFortified){

  if (!canUnitAttackUnit(attacker, defender)) return 0;

  UnitTraits attackerTraits = UnitTraits::traitsForUnitType(attacker);
  UnitTraits defenderTraits = UnitTraits::traitsForUnitType(defender);

  // Raw damage is twice the attack power, scaled by the attackers health
  // (rounded up so a hurt unit still hits).
  uint8_t damage = (attackerTraits.attackPower*2*attackerHealth + UnitDefaultHealth-1) / UnitDefaultHealth;

  // Defense of the unit plus cover. Air units get no cover from terrain.
  uint8_t reduction = defenderTraits.defense;
  if (!isAir(defender)) reduction += mapTileDefenseBonus(defenderTile);
  if (defenderFortified) reduction += FortifyBonus;

  // Every successful attack deals at least 1 damage.
  return (damage > reduction) ? (damage - reduction) : 1;
}

UnitTraits UnitTraits::traitsForUnitType(UnitType unitType){
  UnitTraits traits;

  pgm_readAnything(&allUnitTraits[static_cast<uint8_t>(unitType)], traits);

  return traits;
}

// ====================================================
// Player

AWPlayer::AWPlayer(uint8_t aid){
  // reset() runs when a game starts
  this->playerID = aid;
}

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

bool AWPlayer::operator==(const AWPlayer& other) const{
    return this->playerID == other.playerID;
}

// ====================================================
// MapTile

bool MapTile::canBeAccessedByUnit(MapTileType mapTileType, UnitType unitType){
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
  return (movement & MapTile::lutMaskForMaptile(mapTileType)) != 0;
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
