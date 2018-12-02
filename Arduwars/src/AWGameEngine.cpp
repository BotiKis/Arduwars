#include "AWGameEngine.h"

#include "AWGameSceneMenu.h"
#include "AWGameSceneMapSelect.h"
#include "AWGameSceneMultiplayer.h"
#include "AWGameSceneSingleplayer.h"

void AWGameEngine::setup(void)
{
    // Setup super
    EngineBoy::setup();

    // Set up text
    context.tinyfont.setTextColor(BLACK);

    // Init information
    context.selectedGameMode = AWGameMode::NotSelected;
    context.selectedMap = MapID::None;

    // Show menu
    this->changeToScene(AWGameState::Menu);
}

void AWGameEngine::didChangeToScene(GameScene<GameContext, GameSceneID> *previousScene, GameScene<GameContext, GameSceneID> *nextScene)
{
    // Clean up
    if(previousScene != nullptr)
      delete previousScene;

    if (nextScene == nullptr)
      {
        context.selectedGameMode = AWGameMode::NotSelected;
        context.selectedMap = MapID::None;
      }
    else if (1) {
      /* code */
    }
}

GameScene<AWGameEngine::GameContext, AWGameEngine::GameSceneID>* AWGameEngine::gameSceneForSceneID(GameSceneID sceneID)
{
    switch(sceneID)
    {
        case AWGameState::MapSelection: return new AWGameSceneMapSelect;
        case AWGameState::Playing:
        {
          switch (context.selectedGameMode) {
            case AWGameMode::Multiplayer:   return new AWGameSceneMultiplayer(this->context.selectedMap);
            case AWGameMode::Singleplayer:  return new AWGameSceneSingleplayer;
            default:                        return new AWGameSceneMenu;
          }
        }

        // default to menu
        case AWGameState::Menu:
        default:  return new AWGameSceneMenu;
    }
}
