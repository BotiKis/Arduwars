#ifndef PGM_READ_ANYTHING
#define PGM_READ_ANYTHING

#include <Arduino.h>  // for type definitions

template <typename T> void pgm_readAnything(const T * sce, T& dest){
  memcpy_P (&dest, sce, sizeof (T));
}

#endif
