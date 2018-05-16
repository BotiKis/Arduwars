#ifndef LOCA_H
#define LOCA_H
#include <avr/pgmspace.h>

// This file contains all text used by the game
// Can be localized

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
char_P LOCA_endTurn[] PROGMEM        = "END TURN";
char_P LOCA_day[] PROGMEM            = "DAY:";
char_P LOCA_funds[] PROGMEM          = "M:";

// Unit names
char_P LOCA_Unit_Soldier[] PROGMEM        = "Soldier";
char_P LOCA_Unit_Mech[] PROGMEM           = "Mech";
char_P LOCA_Unit_SpecOps[] PROGMEM        = "SpecOps";

char_P LOCA_Unit_Recon[] PROGMEM          = "Recon";
char_P LOCA_Unit_Assist[] PROGMEM         = "Assist";
char_P LOCA_Unit_Tank[] PROGMEM           = "Tank";
char_P LOCA_Unit_BigTank[] PROGMEM        = "BigTank";
char_P LOCA_Unit_Artillery[] PROGMEM      = "Artillery";
char_P LOCA_Unit_Rocket[] PROGMEM         = "Rocket";
char_P LOCA_Unit_Missiles[] PROGMEM       = "Missiles";

char_P LOCA_Unit_Heli[] PROGMEM           = "Heli";
char_P LOCA_Unit_Fighter[] PROGMEM        = "Fighter";
char_P LOCA_Unit_Bomber[] PROGMEM         = "Bomber";

char_P LOCA_Unit_Cruiser[] PROGMEM        = "Cruiser";
char_P LOCA_Unit_Battleship[] PROGMEM     = "Battleship";
char_P LOCA_Unit_Transportships[] PROGMEM = "Transportships";

// all units toghether
char_P* const LOCA_Unit_Names[] PROGMEM = {
  LOCA_Unit_Soldier,
  LOCA_Unit_Mech,
  LOCA_Unit_SpecOps,

  LOCA_Unit_Recon,
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
