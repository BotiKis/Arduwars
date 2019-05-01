#pragma once

#include "EngineBoy.h"
#include "AWGameContext.h"

class AWGameSceneMapSelect : public GameScene<AWGameContext, AWGameState>{
public:
    // In this variable we will store the index of the cursor so
    // we know what the player has selected.
    int8_t cursorIdx = 0;

public:
    AWGameSceneMapSelect();

    void update(EngineBoy<GameContext, GameSceneID> & engine) override;
    void render(EngineBoy<GameContext, GameSceneID> & engine) override;
};
