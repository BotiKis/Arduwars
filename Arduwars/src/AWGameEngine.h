#pragma once

#include "EngineBoy.h"
#include "AWGameState.h"
#include "AWGameContext.h"

// Global variables
constexpr uint8_t SCROLLSPEED_NORMAL = 1;
constexpr uint8_t SCROLLSPEED_FAST = 3;
constexpr uint8_t TILE_SIZE = 16;

class AWGameEngine: public EngineBoy <AWGameContext, AWGameState>
{
public:
  AWGameContext context;

public:
    void setup(void) override;

    GameContext & getContext(void) override
    {
      return this->context;
    }

    const GameContext & getContext(void) const override
    {
      return this->context;
    }
    
private:

    GameScene<GameContext, GameSceneID>* gameSceneForSceneID(GameSceneID sceneID) override;
    void didChangeToScene(GameScene<GameContext, GameSceneID> *previousScene, GameScene<GameContext, GameSceneID> *nextScene) final;
};
