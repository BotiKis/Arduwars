#pragma once

#include "EngineBoy.h"
#include <Tinyfont.h>
#include <Sprites.h>
#include <List.h>

#include "DataClasses.h"
#include "AWGameState.h"
#include "MapData.h"

class AWGameContext
{
public:
  // This is our instance of the Sprites class.
  // We need this to draw tilesets and animations which Arduboy2 can not do.
  SpritesB sprites;

  // This is tinyfont, a small font we use to print text on the display.
  // It is much smaller that Arduboy2s font so we can fit it better on the screen.
  // The disadvantage is the poor readability.
  Tinyfont tinyfont = Tinyfont(Arduboy2Base::sBuffer, Arduboy2::width(), Arduboy2::height());

  // This variable holds the selected gamemode
  AWGameMode selectedGameMode;

  // This variable holds the selected gamemode
  MapID selectedMap;
};
