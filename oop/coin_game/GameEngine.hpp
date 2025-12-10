#pragma once
#include "GameField.hpp"
#include "Robot.hpp"
#include "RobotFactory.hpp"
#include "GameVisualizer.hpp"
#include "GameTypes.hpp"
#include <memory>
#include <vector>
#include <iostream>

class GameEngine
{
private:
    std::unique_ptr<GameField> _field;
    GameConfig _config;
    int _currentTurn;
    GameVisualizer _visualizer;

public:
    GameEngine(const GameConfig &config) : _config(config), _currentTurn(0) {}

    // Основные методы
    void Initialize();
    void ExecuteTurn();
    void RunGame();

    // Проверка окончания игры
    bool IsGameOver() const;

    // Геттеры
    const GameField &GetField() const { return *_field; }
    const GameConfig &GetConfig() const { return _config; }
    int GetCurrentTurn() const { return _currentTurn; }

    // Получение роботов через поле
    std::vector<Robot *> GetRobots() const { return _field->GetAllRobots(); }
};