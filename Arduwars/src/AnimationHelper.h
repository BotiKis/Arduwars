#ifndef ANIMATIONHELPER_H
#define ANIMATIONHELPER_H

#include <Arduboy2.h>

// returns a point between the start and endpoint where the progress must be between 0 and 100
// The result is lineary interpolated between the origin and the destination
static inline Point animatePointFromToPoint(Point origin, Point destination, uint8_t progress){
  Point result;

  result.x = origin.x + ((destination.x - origin.x)*progress)/100;
  result.y = origin.y + ((destination.y - origin.y)*progress)/100;

  return result;
}

// helper to calculate deltatime (a time difference to the current moment)
#define MILLIS_SINCE(MILLIS) (millis() - MILLIS)

#endif
