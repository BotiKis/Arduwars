#include "AWGameSceneMenu.h"

#include "SpriteAssets.h"
#include "Localization.h"
#include "FlashString.h"

AWGameSceneMenu::AWGameSceneMenu()
{
  this->gameSceneID = AWGameState::Menu;
}

void AWGameSceneMenu::update(EngineBoy<GameContext, GameSceneID> & engine)
{
    // Here we limit and wrap the cursor so it cannot be larger than 2 nor smaller then 0.
    if (engine.arduboy.justPressed(UP_BUTTON))
      cursorIdx--;

    if (engine.arduboy.justPressed(DOWN_BUTTON))
      cursorIdx++;

    cursorIdx = abs(cursorIdx)%2;

    if (engine.arduboy.justPressed(B_BUTTON)){

      // React to users choice
      switch (cursorIdx) {
        case 0:
        {
          engine.getContext().selectedGameMode = AWGameMode::Singleplayer;
          engine.changeToScene(AWGameState::MapSelection);
          break;
        }
        case 1:
        {
          engine.getContext().selectedGameMode = AWGameMode::Multiplayer;
          engine.changeToScene(AWGameState::MapSelection);
          break;
        }
        default: // this default is not needed but it's safe to do this.
        {
          engine.getContext().selectedGameMode = AWGameMode::NotSelected;
          break;
        }
      }
    }
}

void AWGameSceneMenu::render(EngineBoy<GameContext, GameSceneID> & engine)
{
    static constexpr uint8_t yOffset = 44;

    // get game context
    auto gameContext = engine.getContext();

    // get Tinyfont
    auto tinyfont = gameContext.tinyfont;

    // draw title logo
    gameContext.sprites.drawSelfMasked(7, 0, arduwarsLogo, 0);

    // Here we draw the menu text
    tinyfont.setTextColor(WHITE);

    tinyfont.setCursor(10, yOffset);
    tinyfont.print(AsFlashString(LOCA_SinglePlayer));

    tinyfont.setCursor(10, yOffset+8);
    tinyfont.print(AsFlashString(LOCA_MultiPlayer));

    tinyfont.setCursor(2, yOffset + 8*cursorIdx);
    tinyfont.print(AsFlashString(LOCA_Cursor));

    tinyfont.setTextColor(BLACK);
}
