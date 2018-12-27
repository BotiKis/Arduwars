#include "AWGameEngine.h"
AWGameEngine arduwarsGame;

void setup()
{
  arduwarsGame.setup();
}

void loop()
{
  if(!arduwarsGame.arduboy.nextFrame())
    return;

  arduwarsGame.update();
  arduwarsGame.display();
}
