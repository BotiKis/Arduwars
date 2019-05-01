#include "AWGameSceneSingleplayer.h"

#include "SpriteAssets.h"
#include "Localization.h"
#include "FlashString.h"

AWGameSceneSingleplayer::AWGameSceneSingleplayer()
{
}

void AWGameSceneSingleplayer::update(EngineBoy<GameContext, GameSceneID> & engine)
{
  if (engine.arduboy.justPressed(B_BUTTON))
    engine.changeToScene(AWGameState::Menu);
}

void AWGameSceneSingleplayer::render(EngineBoy<GameContext, GameSceneID> & engine)
{
  // get game context
  auto & gameContext = engine.getContext();

  // get Tinyfont
  auto & tinyfont = gameContext.tinyfont;

  // Here we draw the menu text
  tinyfont.setTextColor(WHITE);

  tinyfont.setCursor(0, 0);
  tinyfont.print("SINGLEPLAYER");

  tinyfont.setTextColor(BLACK);
}
