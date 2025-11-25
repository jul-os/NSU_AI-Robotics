#pragma once
#include <cmath>

enum class Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    STAY
};

struct Position
{
    int x, y;

    bool operator==(const Position &other) const
    {
        return x == other.x && y == other.y;
    }

    int DistanceTo(const Position &other) const
    {
        return std::abs(x - other.x) + std::abs(y - other.y);
    }

    Position GetNeighbor(Direction dir) const
    {
        switch (dir)
        {
        case Direction::UP:
            return {x, y - 1};
        case Direction::DOWN:
            return {x, y + 1};
        case Direction::LEFT:
            return {x - 1, y};
        case Direction::RIGHT:
            return {x + 1, y};
        case Direction::STAY:
            return {x, y};
        }
        return *this;
    }
};