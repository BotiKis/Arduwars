#ifndef MAPDATA_H
#define MAPDATA_H

#include <avr/pgmspace.h>

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
// Copy and paste the all contents in here.
//
// IMPORTANT NOTE:
// Don't use the sprites 32 and 33 (Tops of the Headquarters) and place a plain (#14) there instead.
// The game will automaticaly place the right tile for you.

const unsigned char PROGMEM mapData24x16[] =
{
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

#endif
