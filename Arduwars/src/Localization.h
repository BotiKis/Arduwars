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


#endif
