#pragma once
#include "Position.hpp"
#include "GameTypes.hpp"
#include <vector>
#include <map>
#include <memory>
#include <random>

class Robot;

class GameField
{
public:
    GameField(size_t w, size_t h);

    // Управление монетами
    void PlaceCoinsRandomly(int numCoins);
    bool HasCoin(const Position &pos) const;
    void RemoveCoin(const Position &pos);
    int GetCoinCount() const;

    // Управление роботами - переработанные методы
    bool PlaceRobot(std::unique_ptr<Robot> robot);
    bool MoveRobot(Robot *robot, const Position &newPos);
    Robot *GetRobotAt(const Position &pos) const;
    void RemoveRobot(const Position &pos);

    std::vector<Position> GetRobotPositions() const;
    std::vector<Position> GetCoinPositions() const;
    std::vector<Robot *> GetAllRobots() const;

    // Валидация позиции
    bool IsValidPosition(const Position &pos) const;
    void Visualize() const;

    // Геттеры
    size_t GetWidth() const { return _width; }
    size_t GetHeight() const { return _height; }

private:
    size_t _width, _height;
    std::vector<std::vector<bool>> _coins;
    std::map<Position, std::unique_ptr<Robot>> _robots;

    mutable std::random_device _rd;
    mutable std::mt19937 _gen;

    // Вспомогательные методы
    Position FindFreePosition() const;
};