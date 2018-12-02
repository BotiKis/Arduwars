#pragma once

#include <avr/pgmspace.h>

#warning "rework these defines"

#define MAPDATAOFFSET_Size 0
#define MAPDATAOFFSET_Player1City 2
#define MAPDATAOFFSET_Player1Workshop 4
#define MAPDATAOFFSET_Player2City 6
#define MAPDATAOFFSET_Player2Workshop 8
#define MAPDATAOFFSET_Main 10

// In this file we store our maps.
// You can create your custom maps with tiled:The Map editor https://www.mapeditor.org/
//
// How to make custom maps:
// Open Tiled and create a new Tileset with the image in Assets/ called "worldSprite_16x16.png"
// The tile height and width should both be 16.
// After that you can create a new map and use the earlier created tilesheet
// to draw a map of your like.
// Next export your map as CSV.
// Open the CSV and add a colon ',' to the end of each line.
// Copy and paste the contents into an array below.
// Add the width and height as the first two values before the map data
//
// IMPORTANT NOTE:
// #1 Don't use the sprites 32 and 33 (Tops of the Headquarters) and place a plain (#14) there instead.
//    The game will automaticaly place the right tile for you.
// #2 Use a maximum of 24 Buildings. (Defined by BuildingType)

enum class MapID
{
  None,
  Map1,
  Map2,
  Map3
};

#warning "change back to type alias"
// using RawMapData = const unsigned char; -> Linter throws error, changed to typedef
typedef const unsigned char RawMapData;

RawMapData PROGMEM mapData_1[] =
{
  // Map size
  24, 16,

  // Player 1 City
  1,3,
  // Player 1 Workshop
  2,1,

  // Player 2 City
  22,12,
  // Player 2 Workshop
  21,14,

  // Map Data
  24,24,24,24,24,24,23,23,23,23,23,23,23,23,23,23,23,23,23,22,24,22,23,23,
  24,24,26,24,24,25,24,23,22,24,24,22,25,22,24,24,24,23,25,24,24,24,22,23,
  24,24,14,14,14,14,24,14,14,14,24,24,14,14,14,14,14,14,14,14,26,24,24,22,
  24,25,14,30,20,16,16,16,16,16,16,17,14,24,24,24,24,24,14,14,14,14,24,24,
  23,27,14,20,19,14,14,24,24,22,2,18,6,7,24,24,23,23,24,14,18,14,24,22,
  23,23,14,18,14,14,14,24,23,2,12,18,11,13,7,22,23,23,22,14,18,14,25,23,
  23,24,14,18,14,14,24,24,2,12,9,14,4,11,13,7,22,23,23,22,18,14,23,23,
  23,24,24,18,14,24,24,24,3,8,25,24,24,4,11,8,24,24,23,24,18,14,24,23,
  23,24,14,18,24,23,24,24,3,13,7,24,24,25,3,8,24,24,24,14,18,24,24,23,
  23,23,14,18,22,23,23,22,4,11,13,7,14,2,10,9,24,24,14,14,18,14,24,23,
  23,25,14,18,14,22,23,23,22,4,11,13,18,10,9,23,24,14,14,20,19,14,23,23,
  22,24,14,18,14,24,23,23,24,24,4,5,18,9,22,24,24,14,14,18,14,14,27,23,
  24,24,14,14,14,14,24,24,24,24,24,14,21,16,16,16,16,16,16,19,31,14,25,24,
  22,24,24,26,14,14,14,14,14,14,14,14,24,24,14,14,14,24,14,14,14,14,24,24,
  23,22,24,24,24,25,23,24,24,24,22,25,22,24,24,22,23,24,25,24,24,26,24,24,
  23,23,22,24,22,23,23,23,23,23,23,23,23,23,23,23,23,23,24,24,24,24,24,24,
};

RawMapData PROGMEM mapData_2[] =
{
  // Map size
  20, 12,

  // Player 1 City
  2,8,
  // Player 1 Workshop
  0,10,

  // Player 2 City
  17,8,
  // Player 2 Workshop
  19,10,

  // Map Data
  23,22,24,25,24,24,24,24,22,23,18,22,24,24,24,24,25,24,22,23,
  24,14,14,14,14,14,14,14,14,14,18,14,14,14,14,14,14,14,14,24,
  24,14,22,24,2,6,6,7,24,14,18,24,2,6,6,7,24,22,14,24,
  24,14,23,2,10,0,0,13,6,6,6,6,10,0,0,13,7,23,14,24,
  24,14,2,10,0,0,1,0,0,0,0,0,0,1,0,0,13,7,14,24,
  24,14,3,0,0,0,0,0,12,5,18,11,0,0,0,0,0,8,14,24,
  25,14,3,0,0,0,0,0,8,24,18,3,0,0,0,0,0,8,14,25,
  24,14,4,11,0,0,0,0,8,24,18,3,0,0,0,0,12,9,14,24,
  24,14,25,3,0,1,0,0,8,25,29,3,0,0,1,0,8,25,14,24,
  24,14,24,4,5,5,11,0,8,24,24,3,0,12,5,5,9,24,14,24,
  26,14,14,14,28,24,3,0,8,24,24,3,0,8,24,28,14,14,14,26,
  24,30,24,24,23,22,3,0,8,24,24,3,0,8,22,23,24,24,31,24,
};

RawMapData PROGMEM mapData_3[] =
{
  // Map size
  12, 12,

  // Player 1 City
  3,0,
  // Player 1 Workshop
  2,1,

  // Player 2 City
  8,11,
  // Player 2 Workshop
  9,10,

  // Map Data
  24,14,24,25,3,0,1,8,23,23,23,23,
  24,30,26,24,4,11,12,9,22,22,26,23,
  24,14,14,24,23,4,9,24,24,24,22,23,
  24,14,14,25,23,23,25,14,14,14,24,23,
  24,14,14,24,23,24,14,14,14,14,14,24,
  24,14,14,23,24,24,24,14,14,14,14,24,
  24,14,14,14,14,24,24,24,23,14,14,24,
  24,14,14,14,14,14,24,23,24,14,14,24,
  23,24,14,14,14,25,23,23,25,14,14,24,
  23,22,24,24,24,2,7,23,24,14,14,24,
  23,26,22,22,2,10,13,7,24,26,31,24,
  23,23,23,23,3,1,0,8,25,24,24,24,
};
