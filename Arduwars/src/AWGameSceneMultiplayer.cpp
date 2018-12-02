#include "AWGameSceneMultiplayer.h"

#include "SpriteAssets.h"
#include "SpriteAnimations.h"
#include "UnitSprites.h"
#include "Tileset.h"

#include "Localization.h"
#include "FlashString.h"
#include "PointMath.h"

AWGameSceneMultiplayer::AWGameSceneMultiplayer(MapID mapID)
{
  this->gameSceneID = AWGameState::Playing;

  // reset players
  this->player1.reset();
  this->player2.reset();

  // load map
  this->loadMap(mapID);

  this->currentplayer = &this->player1;
}

void AWGameSceneMultiplayer::update(EngineBoy<GameContext, GameSceneID> & engine, uint32_t deltaTime)
{

}

void AWGameSceneMultiplayer::render(EngineBoy<GameContext, GameSceneID> & engine)
{
  // get game context
  auto gameContext = engine.getContext();

  // get Tinyfont
  auto tinyfont = gameContext.tinyfont;
}

void AWGameSceneMultiplayer::didBecomeInActive(EngineBoy<GameContext, GameSceneID> & engine)
{
  // clean up
  if (this->mapTileData != nullptr) {
    delete this->mapTileData;
    this->mapTileData = nullptr;
  }
}

void AWGameSceneMultiplayer::loadMap(MapID mapID){
  RawMapData *mapData = nullptr;

  // get actual map data
  switch (mapID)
  {
    case MapID::Map1: mapData = mapData_1; break;
    case MapID::Map2: mapData = mapData_2; break;
    case MapID::Map3: mapData = mapData_3; break;
    default: break;
    #warning "handle no data error - don't think it's neccessary"
  }

  // handle map meta info
  // get size
  this->mapSize.x  = pgm_read_byte(mapData+MAPDATAOFFSET_Size);
  this->mapSize.y  = pgm_read_byte(mapData+MAPDATAOFFSET_Size+1);

  // p1 meta Data
  Point player1StartCityCoords;
  player1StartCityCoords.x = pgm_read_byte(mapData+MAPDATAOFFSET_Player1City);
  player1StartCityCoords.y = pgm_read_byte(mapData+MAPDATAOFFSET_Player1City+1);

  Point player1StartWorkshopCoords;
  player1StartWorkshopCoords.x = pgm_read_byte(mapData+MAPDATAOFFSET_Player1Workshop);
  player1StartWorkshopCoords.y = pgm_read_byte(mapData+MAPDATAOFFSET_Player1Workshop+1);

  // p2 meta Data
  Point player2StartCityCoords;
  player2StartCityCoords.x = pgm_read_byte(mapData+MAPDATAOFFSET_Player2City);
  player2StartCityCoords.y = pgm_read_byte(mapData+MAPDATAOFFSET_Player2City+1);

  Point player2StartWorkshopCoords;
  player2StartWorkshopCoords.x = pgm_read_byte(mapData+MAPDATAOFFSET_Player2Workshop);
  player2StartWorkshopCoords.y = pgm_read_byte(mapData+MAPDATAOFFSET_Player2Workshop+1);

  //// Load map data
  // Calculate map lenght
  uint16_t mapLenght = mapSize.x*mapSize.y;

  // create new array of maptiles
  this->mapTileData = new MapTile[mapLenght];

  // The index in the loop will be stored here
  Point currentIndex;

  // Populate array from map data
  for (uint16_t i = 0; i < mapLenght; i++) {

    // get Tile Data
    mapTileData[i].tileID = pgm_read_byte(mapData+MAPDATAOFFSET_Main+i);
    mapTileData[i].buildingIsOccupied = 0;
    mapTileData[i].buildingBelongsTo = 0;
    mapTileData[i].hasUnit = 0;
    mapTileData[i].unitBelongsTo = 0;

    MapTileType tileType = static_cast<MapTileType>(mapTileData[i].tileID);

    // calc index
    currentIndex.x = i % mapSize.x;
    currentIndex.y = i / mapSize.x;

    // Check if it's a building
    if (mapTileIndexIsBuilding(tileType)) {
      GameBuilding building = GameBuilding();

      // set upd building
      building.mapPosX = currentIndex.x;
      building.mapPosY = currentIndex.y;
      building.isOccupied = 0;
      building.belongsToPlayer = 0;
      building.buildingType = mapTileData[i].tileID;

      // Check for city
      if (tileType == MapTileType::City) {
        // check for ownership
        if (currentIndex == player1StartCityCoords) {
          building.isOccupied = 1;
          building.belongsToPlayer = MapTile::BelongsToPlayer1;
        }
        else if (currentIndex == player2StartCityCoords) {
          building.isOccupied = 1;
          building.belongsToPlayer = MapTile::BelongsToPlayer2;
        }
      }

      // Check for Factory
      if (tileType == MapTileType::Factory) {
        // check for ownership
        if (currentIndex == player1StartWorkshopCoords) {
          building.isOccupied = 1;
          building.belongsToPlayer = MapTile::BelongsToPlayer1;
        }
        else if (currentIndex == player2StartWorkshopCoords) {
          building.isOccupied = 1;
          building.belongsToPlayer = MapTile::BelongsToPlayer2;
        }
      }

      // Check for Headquarters
      if (tileType == MapTileType::P1HQ){
        building.isOccupied = 1;
        building.belongsToPlayer = MapTile::BelongsToPlayer1;
        player1.cursorIndex = currentIndex;
      }
      else if (tileType == MapTileType::P2HQ){
        building.isOccupied = 1;
        building.belongsToPlayer = MapTile::BelongsToPlayer2;
        player2.cursorIndex = currentIndex;
      }

      // add building to our global buildings
      gameBuildings.add(building);
    }

  }
}


void AWGameSceneMultiplayer::makeScreenTransition(void)
{
  uint8_t *dbuff = Arduboy2::sBuffer;
  const uint8_t screenWidth = Arduboy2::width();

  for (uint8_t i = 0; i < screenWidth; i++) {
      for (uint8_t x = 0; x < screenWidth; x++) {
        for (uint8_t y = 0; y < 8; y++) {
          if (y%2==0) {
            if (x == screenWidth-1)
              dbuff[x+y*screenWidth] = 0;
            else
              dbuff[x+y*screenWidth] = dbuff[x+y*screenWidth+1];
          }
          else{
            uint8_t helperX = screenWidth-1-x;
            if (helperX == 0)
              dbuff[helperX+y*screenWidth] = 0;
            else
              dbuff[helperX+y*screenWidth] = dbuff[helperX+y*screenWidth-1];
          }
        }
      }

      //arduboy.display();
      #warning "figure out how to call"
      delay(1);
  }
}

void AWGameSceneMultiplayer::showDialog(char_P *titleText){

  const uint8_t screenWidth = Arduboy2::width();

  // frame for the dialog
  Rect frame;

  frame.width = strlen_P(titleText)*5+8;
  frame.height = 16;
  frame.x = (screenWidth - frame.width)/2;
  frame.y = 24;

  Point textPos;
  textPos.x = (screenWidth - strlen_P(titleText)*5)/2;
  textPos.y = frame.y+6;


  #warning "figure out how to call"

  // // dialog loop
  // while (true) {
  //
  //   // wait for next frame
  //   if(!arduboy.nextFrame()) continue;
  //
  //   // Get input
  //   arduboy.pollButtons();
  //
  //   // Exit on B
  //   if (arduboy.justPressed(B_BUTTON)){
  //     return;
  //   }
  //
  //   // Drawing
  //   // Infobox
  //   arduboy.fillRoundRect(frame.x, frame.y, frame.width, frame.height, 5, BLACK);
  //   arduboy.fillRoundRect(frame.x + 1, frame.y + 1, frame.width-2, frame.height-3, 5, WHITE);
  //
  //   // OK Button
  //   tinyfont.setCursor(textPos.x, textPos.y);
  //   tinyfont.print(AsFlashString(titleText));
  //
  //   arduboy.display();
  // }
}


void AWGameSceneMultiplayer::drawMapAtPosition(Point aPosition, EngineBoy<GameContext, GameSceneID> & engine)
{
  // helpers
  const uint8_t screenWidth = Arduboy2::width();
  const uint8_t screenHeight = Arduboy2::height();

  // get game context
  auto gameContext = engine.getContext();

  // In this variable we store the tile
  Point drawPos;

  // draw the map
  for (int8_t y = 0; y < mapSize.y; y++) {
    for (int8_t x = mapSize.x-1; x >= 0; x--) { // draw the map from right to left helps with the markers

      drawPos.x = aPosition.x+x*TILE_SIZE;
      drawPos.y = aPosition.y+y*TILE_SIZE;

      // ignore if out of bounds
      if (drawPos.x <= -TILE_SIZE || drawPos.x >= screenWidth || drawPos.y <= -TILE_SIZE || drawPos.y >= (screenHeight+TILE_SIZE)) continue;

      // get the tile
      const MapTile tile = mapTileData[y*mapSize.x+x];
      const MapTileType tileType = static_cast<MapTileType>(tile.tileID);

      // draw maptile
      gameContext.sprites.drawSelfMasked(drawPos.x, drawPos.y, worldSprite, tile.tileID);

      // check if tile below is the HQ
      if (y < mapSize.y-1) {
        const MapTile tileBelow = mapTileData[((y+1)*mapSize.x)+x];
        const MapTileType tileTypeOfTileBelow = static_cast<MapTileType>(tileBelow.tileID);

        // If it's a headquarter draw upper half at the position above
        if (tileTypeOfTileBelow == MapTileType::P1HQ || tileTypeOfTileBelow == MapTileType::P2HQ) {
          gameContext.sprites.drawOverwrite(drawPos.x, drawPos.y, worldSprite, (tileTypeOfTileBelow == MapTileType::P1HQ)?32:33);
        }
      }

      // draw fog
      if (tile.showsFog && !tile.showSelection) {
          gameContext.sprites.drawErase(drawPos.x, drawPos.y, mapFOG_16x16, 0);
      }

      // Draw marker for buildings
      // we need to identify visually to whom the buiding belongs.
      // Since we have no colors to indicate this, we draw small markers at
      // the top right corner of a building.
      else if (mapTileIndexIsBuilding(tileType) && tileType != MapTileType::P1HQ && tileType != MapTileType::P2HQ) {
        if (tile.buildingIsOccupied){
          if(tile.buildingBelongsTo == MapTile::BelongsToPlayer){
            gameContext.sprites.drawPlusMask(drawPos.x+10, drawPos.y-2, mapMarkers_plus_mask, 0);
          }
          else if(tile.buildingBelongsTo == MapTile::BelongsToEnemy){
            gameContext.sprites.drawPlusMask(drawPos.x+10, drawPos.y-2, mapMarkers_plus_mask, 1);
          }
        }
      }

      // draw unit selection
      if (tile.showSelection) {
        gameContext.sprites.drawErase(drawPos.x, drawPos.y, selectionAnimation, (engine.arduboy.frameCount/10)%4);
      }

      // Draw Unit
      if(tile.hasUnit && !tile.showsFog){
        // unitSprite
        const unsigned char *unitSprite = nullptr;

        // get correct sprite
        if(tile.unitBelongsTo == MapTile::BelongsToPlayer1)
          unitSprite = unitsA_plus_mask;
        else if(tile.unitBelongsTo == MapTile::BelongsToPlayer2)
          unitSprite = unitsB_plus_mask;

        // Check if unit is active
        if (tile.unitIsActive == GameUnit::UnitStateActive) {
          // Draw sprite
          // The *2 is because every unit in the spritesheet has two frames
          // The +(arduboy.frameCount/10)%2 part switches between these two frames
          // based on the current framecount.
          gameContext.sprites.drawPlusMask(drawPos.x, drawPos.y, unitSprite, tile.unitSpriteID*2+(engine.arduboy.frameCount/10)%2);
        }
        else{
          // Don't animate if not active and draw a badge to top right.
          // The *2 is because every unit in the spritesheet has two frames
          gameContext.sprites.drawPlusMask(drawPos.x, drawPos.y, unitSprite, tile.unitSpriteID*2);
          gameContext.sprites.drawPlusMask(drawPos.x+10, drawPos.y-2, mapMarkers_plus_mask, 2);
        }

      }

    }
  }
}
