#pragma once
#include "Position.hpp"
#include "GameTypes.hpp"
#include <string>
#include <memory>

class GameField;
class Robot;
// роботам надо знать текущее положение игры чтобы решить куда двигаться
struct GameState
{
    GameField &field;
    const std::vector<Robot *> &allRobots;
    const Robot &currentRobot;
    int currentTurn;
    int totalTurns;

    GameState(GameField &f, const std::vector<Robot *> &robots,
              const Robot &current, int turn, int total)
        : field(f), allRobots(robots), currentRobot(current),
          currentTurn(turn), totalTurns(total)
    {
    }
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
    void Move(Direction dir, GameState &state);

    // Метод подбора монетки
    void CollectCoin()
    {
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

// Двигается в рандомном направлении
class RandomRobot : public Robot
{
public:
    RandomRobot() : Robot("RandomRobot") {}
    Direction DecideMove(const GameState &state) override;
};

// Жадно собирает монетки
class GreedyRobot : public Robot
{
public:
    GreedyRobot() : Robot("GreedyRobot") {}
    Direction DecideMove(const GameState &state) override;
};

// Осторожный робот - избегает других роботов
class CautiousRobot : public Robot
{
public:
    CautiousRobot() : Robot("CautiousRobot") {}
    Direction DecideMove(const GameState &state) override;
};

// Патрулирующий робот - движется по маршруту
class PatrollingRobot : public Robot
{
private:
    bool _movingRight = true;
    bool _movingDown = true;

public:
    PatrollingRobot() : Robot("PatrollingRobot") {}
    Direction DecideMove(const GameState &state) override;
};

// Умный робот - комбинированная стратегия
class SmartRobot : public Robot
{
public:
    SmartRobot() : Robot("SmartRobot") {}
    Direction DecideMove(const GameState &state) override;
};

// Агрессивный робот - пытается мешать другим
class AggressiveRobot : public Robot
{
public:
    AggressiveRobot() : Robot("AggressiveRobot") {}
    Direction DecideMove(const GameState &state) override;
};