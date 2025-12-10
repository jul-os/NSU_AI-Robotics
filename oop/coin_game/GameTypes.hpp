#pragma once
#include "Position.hpp"
#include <cstddef>
#include <vector>
#include <string>

enum class GameMode
{
    Detailed,
    Fast,
    Tournament
};

struct GameConfig
{
    GameMode mode;
    size_t fieldWidth;
    size_t fieldHeight;
    int coinCount;
    int maxTurns;
    std::vector<std::string> strategies;
    int gameCount;
};