#pragma once
#include "GameField.hpp"
#include "Robot.hpp"
#include "RobotFactory.hpp"
#include "GameTypes.hpp"
#include <memory>
#include <vector>

class GameEngine
{
private:
    std::unique_ptr<GameField> _field;
    std::vector<std::unique_ptr<Robot>> _robots;
    GameConfig _config;
    int _currentTurn;

public:
    GameEngine(const GameConfig &config) : _config(config), _currentTurn(0) {}

    void Initialize()
    {
        _field = std::make_unique<GameField>(_config.fieldWidth, _config.fieldHeight);
        _field->PlaceCoinsRandomly(_config.coinCount);

        _robots.clear();
        for (const auto &strategyName : _config.strategies)
        {
            auto robot = RobotFactory::CreateRobot(strategyName);
            _field->PlaceRobotRandomly(robot.get());
            _robots.push_back(std::move(robot));
        }
    }

    void ExecuteTurn()
    {
        for (auto &robot : _robots)
        {
            // Создаем состояние игры для текущего робота
            std::vector<Robot *> robotPtrs;
            for (auto &r : _robots)
                robotPtrs.push_back(r.get());

            GameState state(*_field, robotPtrs, *robot, _currentTurn, _config.maxTurns);

            // Робот принимает решение
            Direction move = robot->DecideMove(state);

            // Выполняем перемещение
            Position oldPos = robot->GetPosition();
            robot->Move(move);
            Position newPos = robot->GetPosition();

            // Обновляем поле
            _field->RemoveRobot(oldPos);
            _field->PlaceRobot(robot.get(), newPos);

            // ask надо ли робота переставлять внутри себя

            // Проверяем сбор монеты
            if (_field->HasCoin(newPos))
            {
                robot->CollectCoin();
                _field->RemoveCoin(newPos);
            }
        }
        _currentTurn++;
    }

    bool IsGameOver() const
    {
        return _currentTurn >= _config.maxTurns;
    }

    // Геттеры
    const GameField &GetField() const { return *_field; }
    const std::vector<std::unique_ptr<Robot>> &GetRobots() const { return _robots; }
    int GetCurrentTurn() const { return _currentTurn; }
};