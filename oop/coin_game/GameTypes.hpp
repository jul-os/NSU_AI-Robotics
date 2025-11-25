#pragma once
#include "Position.hpp"
#include <cstddef>
#include <vector>
#include <string>
// ask не надо ли конфиг вставить в coinGame
struct GameConfig
{
    size_t fieldWidth;
    size_t fieldHeight;
    int coinCount;
    int maxTurns;
    std::vector<std::string> strategies;
    std::string mode;
    int gameCount;
};