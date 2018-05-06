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
  17,17,17,17,17,17,16,16,16,16,16,16,16,16,16,16,16,16,16,15,17,15,16,16,
  17,17,17,17,17,25,17,16,15,17,17,15,25,15,17,17,17,16,25,17,17,17,15,16,
  17,17,14,14,14,14,17,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,17,15,
  17,25,14,30,23,19,19,19,19,19,19,20,14,17,17,17,17,17,14,14,26,14,17,17,
  16,16,14,23,22,14,14,17,17,15,2,21,6,7,17,17,16,16,17,14,14,14,17,15,
  16,14,14,21,14,14,14,17,16,2,12,21,11,13,7,15,16,16,15,14,21,14,25,16,
  16,14,14,21,14,14,14,17,2,12,9,14,4,11,13,7,15,16,16,15,21,14,16,16,
  16,27,14,21,14,14,17,17,3,8,17,14,25,4,11,8,17,17,16,17,21,14,17,16,
  16,17,14,21,17,16,17,17,3,13,7,25,14,17,3,8,17,17,14,14,21,14,27,16,
  16,16,14,21,15,16,16,15,4,11,13,7,14,2,10,9,17,14,14,14,21,14,14,16,
  16,25,14,21,14,15,16,16,15,4,11,13,21,10,9,16,17,14,14,23,22,14,14,16,
  15,17,14,14,14,17,16,16,17,17,4,5,21,9,15,17,17,14,14,21,14,14,16,16,
  17,17,14,26,14,14,17,17,17,17,17,14,24,19,19,19,19,19,19,22,31,14,25,17,
  15,17,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,17,14,14,14,14,17,17,
  16,15,17,17,17,25,16,17,17,17,15,25,15,17,17,15,16,17,25,17,17,17,17,17,
  16,16,15,17,15,16,16,16,16,16,16,16,16,16,16,16,16,16,17,17,17,17,17,17,
};

#endif
