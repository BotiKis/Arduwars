#include "AWGameEngine.h"
AWGameEngine arduwarsGame;

void setup()
{
  // put your setup code here, to run once:
  arduwarsGame.setup();
}

void loop()
{
  // put your main code here, to run repeatedly:

  if(!arduwarsGame.arduboy.nextFrame())
    return;

  arduwarsGame.update();
  arduwarsGame.display();
}
