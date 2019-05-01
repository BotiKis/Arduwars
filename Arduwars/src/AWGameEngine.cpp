#include "AWGameEngine.h"
#include "MemoryFree.h"
#include "PlacementNew.h"


void AWGameEngine::setup(void)
{
    // Setup super
    EngineBoy::setup();

    // Set up text
    this->getContext().tinyfont.setTextColor(BLACK);

    // Init information
    this->getContext().setSelectedGameMode(AWGameMode::NotSelected);
    this->getContext().setSelectedMap(MapID::None);

    // Show menu
    this->changeToScene(AWGameState::Menu);
}

void AWGameEngine::_render(void)
{
  // get Tinyfont
  auto & tinyfont = this->getContext().tinyfont;

  tinyfont.setTextColor(WHITE);

  this->arduboy.fillRect(82, 57, 48, 8, WHITE);
  this->arduboy.fillRect(83, 58, 47, 6, BLACK);

  tinyfont.setCursor(84,59);
  tinyfont.print(F("Free:"));
  tinyfont.setCursor(84+24,59);
  tinyfont.print(freeMemory());

  tinyfont.setTextColor(BLACK);
}

void AWGameEngine::willShowScene(GameScene<GameContext, GameSceneID> *nextScene)
{
  if (nextScene == nullptr)
  {
    context.setSelectedGameMode(AWGameMode::NotSelected);
    context.setSelectedMap(MapID::None);
  }
}

void AWGameEngine::didDismissScene(GameScene<GameContext, GameSceneID> *previousScene)
{
	// Clean up
	if(previousScene != nullptr)
	{
		previousScene->~GameScene();
	}
}

GameScene<AWGameEngine::GameContext, AWGameEngine::GameSceneID>* AWGameEngine::gameSceneForSceneID(GameSceneID sceneID)
{
    switch(sceneID)
    {
        case AWGameState::MapSelection: return new (&this->_gameScenes[0]) AWGameSceneMapSelect();
        case AWGameState::Playing:
        {
          switch (context.selectedGameMode()) {
            case AWGameMode::Multiplayer:   return new (&this->_gameScenes[0]) AWGameSceneMultiplayer(this->getContext().selectedMap());
            case AWGameMode::Singleplayer:  return new (&this->_gameScenes[0]) AWGameSceneSingleplayer();
            default:                        return new (&this->_gameScenes[0]) AWGameSceneMenu();
          }
        }

        // default to menu
        case AWGameState::Menu:
        default:  return new (&this->_gameScenes[0]) AWGameSceneMenu();
    }
}
