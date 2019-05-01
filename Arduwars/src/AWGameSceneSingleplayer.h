#pragma once

#include "EngineBoy.h"
#include "AWGameContext.h"

class AWGameSceneSingleplayer : public GameScene<AWGameContext, AWGameState>{
public:

public:
    AWGameSceneSingleplayer();

    void update(EngineBoy<GameContext, GameSceneID> & engine) override;
    void render(EngineBoy<GameContext, GameSceneID> & engine) override;
};
