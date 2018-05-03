#ifndef POINTMATH_H
#define POINTMATH_H

#include <Arduboy2.h>

using Vector2D = Point;

// Overload == perator for comparisson
static inline bool operator==(const Vector2D &lhs, const Vector2D &rhs){
    return (lhs.x == rhs.x && lhs.y == rhs.y);
}

//Other maths
static inline Vector2D operator+(const Vector2D &lhs, const Vector2D &rhs){
    return {lhs.x+rhs.x, lhs.y+rhs.y};
}

static inline Vector2D operator-(const Vector2D &lhs, const Vector2D &rhs){
    return {lhs.x-rhs.x, lhs.y-rhs.y};
}

// multyply with an skalar
static inline Vector2D operator*(const Vector2D &lhs, const int16_t &rhs){
    return {lhs.x*rhs, lhs.y*rhs};
}

static inline Vector2D operator/(const Vector2D &lhs, const int16_t &rhs){
    return {lhs.x/rhs, lhs.y/rhs};
}

#endif
