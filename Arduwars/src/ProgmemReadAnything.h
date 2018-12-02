#pragma once

#include <avr/pgmspace.h> // for memcpy_P

template <typename T> void pgm_readAnything(const T * sce, T& dest){
  memcpy_P (&dest, sce, sizeof (T));
}