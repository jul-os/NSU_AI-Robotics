#pragma once
#include "Position.hpp"
#include "GameTypes.hpp"
#include <vector>
#include <map>
#include <memory>

class Robot; // forward declaration

class GameField
{

public:
    GameField(size_t w, size_t h);

    // Управление монетами
    void PlaceCoinsRandomly(int numCoins);
    bool HasCoin(const Position &pos) const;
    void RemoveCoin(const Position &pos);
    int GetCoinCount() const;

    // Управление роботами
    bool PlaceRobot(Robot *robot, Position pos);
    bool PlaceRobotRandomly(Robot *robot);
    void RemoveRobot(const Position &pos);
    Robot *GetRobotAt(const Position &pos) const;

    std::vector<Position> GetRobotPositions() const;
    std::vector<Position> GetCoinPositions() const;

    // Валидация и визуализация
    bool IsValidPosition(const Position &pos) const;
    void Visualize() const; // fixme в отдельном

    // Геттеры
    size_t GetWidth() const { return _width; }
    size_t GetHeight() const { return _height; }

private:
    size_t _width, _height;
    std::vector<std::vector<bool>> _coins;
    std::map<Position, Robot *> _robots;
};