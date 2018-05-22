#ifndef LOCA_H
#define LOCA_H
#include <avr/pgmspace.h>

// This file contains all text used by the game
// Can be localized

// used to indicate that the string is store in PROGMEM
using char_P = const char;

// MENU
char_P LOCA_SinglePlayer[] PROGMEM    = "SINGLE PLAYER";
char_P LOCA_MultiPlayer[] PROGMEM     = "TWO PLAYER";
char_P LOCA_Options[] PROGMEM         = "OPTIONS";
char_P LOCA_BackWithA[] PROGMEM       = "< BACK WITH A";

// Map selection
char_P LOCA_mapSmall[] PROGMEM        = "SKIRMISH";
char_P LOCA_mapMedium[] PROGMEM       = "BATTLE";
char_P LOCA_mapBig[] PROGMEM          = "ARDUWARS";

// Playernames
char_P LOCA_player1[] PROGMEM        = "PLAYER 1";
char_P LOCA_player2[] PROGMEM        = "PLAYER 2";

// Game
char_P LOCA_endTurn[] PROGMEM             = "END TURN";
char_P LOCA_day[] PROGMEM                 = "DAY:";
char_P LOCA_funds[] PROGMEM               = "M:";
char_P LOCA_Unit_limit_reached[] PROGMEM  = "UNIT LIMIT REACHED";

// Unit Traits
char_P LOCA_Trait_attack[] PROGMEM   = "ATK";
char_P LOCA_Trait_defense[] PROGMEM  = "DEF";
char_P LOCA_Trait_speed[] PROGMEM    = "SPEED";
char_P LOCA_Trait_range[] PROGMEM    = "RANGE";

// Unit names
char_P LOCA_Unit_Soldier[] PROGMEM        = "SOLDIER";
char_P LOCA_Unit_Mech[] PROGMEM           = "MECH";
char_P LOCA_Unit_SpecOps[] PROGMEM        = "SPECOPS";

char_P LOCA_Unit_scout[] PROGMEM          = "SCOUT";
char_P LOCA_Unit_Assist[] PROGMEM         = "ASSIST";
char_P LOCA_Unit_Tank[] PROGMEM           = "TANK";
char_P LOCA_Unit_BigTank[] PROGMEM        = "BIGTANK";
char_P LOCA_Unit_Artillery[] PROGMEM      = "ARTILLERY";
char_P LOCA_Unit_Rocket[] PROGMEM         = "ROCKET";
char_P LOCA_Unit_Missiles[] PROGMEM       = "MISSILES";

char_P LOCA_Unit_Heli[] PROGMEM           = "HELI";
char_P LOCA_Unit_Fighter[] PROGMEM        = "FIGHTER";
char_P LOCA_Unit_Bomber[] PROGMEM         = "BOMBER";

char_P LOCA_Unit_Cruiser[] PROGMEM        = "CRUISER";
char_P LOCA_Unit_Battleship[] PROGMEM     = "BATTLESHIP";
char_P LOCA_Unit_Transportships[] PROGMEM = "TRANSPORT";

// all units toghether
char_P* const LOCA_Unit_Names[] PROGMEM = {
  LOCA_Unit_Soldier,
  LOCA_Unit_Mech,
  LOCA_Unit_SpecOps,

  LOCA_Unit_scout,
  LOCA_Unit_Assist,
  LOCA_Unit_Tank,
  LOCA_Unit_BigTank,
  LOCA_Unit_Artillery,
  LOCA_Unit_Rocket,
  LOCA_Unit_Missiles,

  LOCA_Unit_Heli,
  LOCA_Unit_Fighter,
  LOCA_Unit_Bomber,

  LOCA_Unit_Cruiser,
  LOCA_Unit_Battleship,
  LOCA_Unit_Transportships
};

#endif
