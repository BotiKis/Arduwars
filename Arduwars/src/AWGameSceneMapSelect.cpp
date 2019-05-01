#include "AWGameSceneMapSelect.h"

#include "SpriteAssets.h"
#include "Localization.h"
#include "FlashString.h"
#include "MapData.h"

AWGameSceneMapSelect::AWGameSceneMapSelect()
{
  
}

void AWGameSceneMapSelect::update(EngineBoy<GameContext, GameSceneID> & engine)
{
  // Here we limit and wrap the cursor so it cannot be larger than 2 nor smaller then 0.
  if (engine.arduboy.justPressed(UP_BUTTON))
    cursorIdx--;

  if (engine.arduboy.justPressed(DOWN_BUTTON))
    cursorIdx++;

  // Limit and wrap the index
  cursorIdx = (cursorIdx<0)?2:cursorIdx;
  cursorIdx = cursorIdx%3;

  if (engine.arduboy.justPressed(B_BUTTON))
  {
    MapID selectedMapID = MapID::None;

    switch (cursorIdx) {
      case 0: selectedMapID = MapID::Map3; break;
      case 1: selectedMapID = MapID::Map2; break;
      case 2: selectedMapID = MapID::Map1; break;
    }

    // get game context
    auto & gameContext = engine.getContext();

    // Set selected map
    gameContext.setSelectedMap(selectedMapID);

    // Safety check then change scene
    if(selectedMapID != MapID::None)
      engine.changeToScene(AWGameState::Playing);
  }

  if (engine.arduboy.justPressed(A_BUTTON))
  {
    engine.changeToScene(AWGameState::Menu);
  }

}

void AWGameSceneMapSelect::render(EngineBoy<GameContext, GameSceneID> & engine)
{
  // needed for layout
  static constexpr uint8_t yOffset = 44;

  // get game context
  auto & gameContext = engine.getContext();

  // get Tinyfont
  auto & tinyfont = gameContext.tinyfont;

  // draw title logo
  gameContext.sprites.drawSelfMasked(7, 0, arduwarsLogo, 0);

  // Set textcolor
  tinyfont.setTextColor(WHITE);

  // Draw the menu
  if (gameContext.selectedGameMode() == AWGameMode::Singleplayer) {
    tinyfont.setCursor(44, 20);
    tinyfont.print(AsFlashString(LOCA_SinglePlayer));
  }
  else if (gameContext.selectedGameMode() == AWGameMode::Multiplayer) {
    tinyfont.setCursor(36, 20);
    tinyfont.print(AsFlashString(LOCA_MultiPlayer));
  }

  tinyfont.setCursor(2, yOffset-8);
  tinyfont.print(AsFlashString(LOCA_BackWithA));

  tinyfont.setCursor(10, yOffset);
  tinyfont.print(AsFlashString(LOCA_mapSmall));
  tinyfont.setCursor(10, yOffset+8);
  tinyfont.print(AsFlashString(LOCA_mapMedium));
  tinyfont.setCursor(10, yOffset+16);
  tinyfont.print(AsFlashString(LOCA_mapBig));

  tinyfont.setCursor(2, yOffset + 8*cursorIdx);
  tinyfont.print(AsFlashString(LOCA_Cursor));

  tinyfont.setTextColor(BLACK);
}
