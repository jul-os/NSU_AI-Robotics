#pragma once
#include "Position.hpp"
#include "GameTypes.hpp"
#include <string>
#include <memory>

class GameField;
// fixme why is it here
struct GameState
{
    const GameField &field;
    const std::vector<Robot *> &allRobots;
    const Robot &currentRobot;
    int currentTurn;
    int totalTurns;

    GameState(const GameField &f, const std::vector<Robot *> &robots,
              const Robot &current, int turn, int total)
        : field(f), allRobots(robots), currentRobot(current),
          currentTurn(turn), totalTurns(total) {}
};

class Robot
{
public:
    Robot(const std::string &robotName)
        : _position{0, 0}, _coinsCollected(0), _movesMade(0),
          _totalDistance(0), _name(robotName) {}

    virtual ~Robot() = default;

    // Основной метод стратегии
    virtual Direction DecideMove(const GameState &state) = 0;

    // Базовые методы перемещения
    void Move(Direction dir);

    // Метод подбора монетки
    void CollectCoin()
    {
        // todo чтобы еще одновременно на поле моентка подбиралась
        _coinsCollected++;
    }

    // поставить робота на позицию pos
    void SetPosition(const Position &pos)
    {
        _position = pos;
    }

    // Геттеры
    Position GetPosition() const { return _position; }
    int GetCoinsCollected() const { return _coinsCollected; }
    int GetMovesMade() const { return _movesMade; }
    int GetTotalDistance() const { return _totalDistance; }
    std::string GetName() const { return _name; }

protected:
    Position _position;
    int _coinsCollected;
    int _movesMade;
    int _totalDistance;
    std::string _name;
};

// Конкретные стратегии

// двигается в рандомном направлении
class RandomRobot : public Robot
{
public:
    RandomRobot() : Robot("RandomRobot") {}
    Direction DecideMove(const GameState &state) override;
};

// жадно собирает монетки
class GreedyRobot : public Robot
{
public:
    GreedyRobot() : Robot("GreedyRobot") {}
    Direction DecideMove(const GameState &state) override;
};

// todo и еще штук 5 или сколько по заданию