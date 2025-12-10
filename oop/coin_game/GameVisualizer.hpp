#pragma once
#include "GameField.hpp"
#include "Robot.hpp"
#include <iostream>
#include <vector>
#include <memory>

class GameVisualizer
{
public:
    // Обновленные методы для работы с vector<Robot*>
    void DisplayField(const GameField &field, int currentTurn = -1, int totalTurns = -1) const;
    void DisplayCurrentResults(const std::vector<Robot *> &robots, int currentTurn = -1) const;
    void DisplayFinalResults(const std::vector<Robot *> &robots) const;
    void DisplayRobotMove(const Robot &robot, Direction move) const;
    void DisplayGameStart(const GameConfig &config) const;
    void DisplayGameEnd() const;

private:
    std::string DirectionToString(Direction dir) const;
};