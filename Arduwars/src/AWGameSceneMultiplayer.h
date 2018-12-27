#pragma once

#include "EngineBoy.h"
#include "AWGameContext.h"
#include "AWGameEngine.h"

#include "MapData.h"
#include "Localization.h"

class AWGameSceneMultiplayer : public GameScene<AWGameContext, AWGameState>{
private:

  // In these two variables we store the players.
  // There are always two players where the first one is the actual player and the second the AI.
  AWPlayer player1 = AWPlayer(1);
  AWPlayer player2 = AWPlayer(2);
  AWPlayer  *currentplayer = nullptr;

  // This attribute stores the days.
  // A day passes if both players has ended their rounds.
  // The game ends with a draw when it reaches 255 days.
  uint8_t daysPlayed = 0;

  // In this attribute we store the buildings on the map - max 16.
  List<GameBuilding, 24> gameBuildings;

  // Map data
  MapTile *mapTileData = nullptr;
  Point mapSize = {0, 0}; // Mapsize is in Maptile coordinates and not in screen coordinates.
  static constexpr uint8_t mapOffsetY = 8;

public:
    AWGameSceneMultiplayer(MapID mapID);

    void update(EngineBoy<GameContext, GameSceneID> & engine) override;
    void render(EngineBoy<GameContext, GameSceneID> & engine) override;
    void didBecomeInActive(EngineBoy<GameContext, GameSceneID> & engine) override;

private:
    void loadMap(MapID mapID);
    void makeScreenTransition(void);
    void showDialog(char_P *titleText);
    void drawMapAtPosition(Point aPosition, EngineBoy<GameContext, GameSceneID> & engine);
};
