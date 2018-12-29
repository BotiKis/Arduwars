#pragma once

#include <Arduino.h>

// These states define the possible states of the game.
enum class AWGameState : uint8_t
{
    Menu,
    MapSelection,
    Playing
};

// These states define the possible states of the game.
enum class AWGameMode : uint8_t
{
    NotSelected,
    Singleplayer,
    Multiplayer
};
