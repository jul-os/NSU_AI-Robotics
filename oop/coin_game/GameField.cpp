#include "GameField.hpp"
#include "Robot.hpp"
#include <iostream>
#include <random>
#include <algorithm>

GameField::GameField(size_t w, size_t h)
    : _width(w > 0 ? w : 5), _height(h > 0 ? h : 5), _gen(_rd())
{
    _coins.resize(_height, std::vector<bool>(_width, false));
    std::cout << "Created game field " << _width << "x" << _height << std::endl;
}

bool GameField::PlaceRobot(std::unique_ptr<Robot> robot)
{
    if (!robot)
    {
        throw std::invalid_argument("Cannot place null robot");
    }

    Position pos = FindFreePosition();
    if (!IsValidPosition(pos))
    {
        std::cout << "No free positions available for robot" << std::endl;
        return false;
    }

    robot->SetPosition(pos);
    _robots[pos] = std::move(robot);

    std::cout << "Placed robot " << _robots[pos]->GetName() << " at position ("
              << pos.x << ", " << pos.y << ")" << std::endl;
    return true;
}

// fixme они не двигаются

bool GameField::MoveRobot(Robot *robot, const Position &newPos)
{
    // Базовая валидация
    if (!robot || !IsValidPosition(newPos))
    {
        return false;
    }

    Position oldPos = robot->GetPosition();

    // Если позиция не изменилась
    if (oldPos == newPos)
    {
        return true;
    }

    // Проверяем, что робот на старой позиции
    auto oldIt = _robots.find(oldPos);
    if (oldIt == _robots.end() || oldIt->second.get() != robot)
    {
        return false;
    }

    // Проверяем, что новая позиция свободна
    if (_robots.find(newPos) != _robots.end())
    {
        return false;
    }

    // Простое перемещение: удаляем со старой, добавляем на новую
    auto robotPtr = std::move(oldIt->second);
    _robots.erase(oldIt);
    _robots[newPos] = std::move(robotPtr);

    // Обновляем позицию робота
    robot->SetPosition(newPos);

    return true;
}
Robot *GameField::GetRobotAt(const Position &pos) const
{
    if (!IsValidPosition(pos))
    {
        return nullptr;
    }

    auto it = _robots.find(pos);
    return (it != _robots.end()) ? it->second.get() : nullptr;
}

void GameField::RemoveRobot(const Position &pos)
{
    if (!IsValidPosition(pos))
    {
        throw std::invalid_argument("Invalid position for RemoveRobot");
    }

    auto it = _robots.find(pos);
    if (it != _robots.end())
    {
        std::cout << "Removed robot " << it->second->GetName() << " from position ("
                  << pos.x << ", " << pos.y << ")" << std::endl;
        _robots.erase(it);
    }
}

std::vector<Robot *> GameField::GetAllRobots() const
{
    std::vector<Robot *> robots;
    for (const auto &pair : _robots)
    {
        robots.push_back(pair.second.get());
    }
    return robots;
}

std::vector<Position> GameField::GetRobotPositions() const
{
    std::vector<Position> positions;
    for (const auto &pair : _robots)
    {
        positions.push_back(pair.first);
    }
    return positions;
}

Position GameField::FindFreePosition() const
{
    // Сначала пытаемся найти случайную свободную позицию
    std::uniform_int_distribution<int> distX(0, _width - 1);
    std::uniform_int_distribution<int> distY(0, _height - 1);

    const int MAX_ATTEMPTS = 100;
    for (int attempt = 0; attempt < MAX_ATTEMPTS; ++attempt)
    {
        Position pos{distX(_gen), distY(_gen)};
        if (_robots.find(pos) == _robots.end())
        {
            return pos;
        }
    }

    // Линейный поиск, если случайный не удался
    for (int y = 0; y < static_cast<int>(_height); ++y)
    {
        for (int x = 0; x < static_cast<int>(_width); ++x)
        {
            Position pos{x, y};
            if (_robots.find(pos) == _robots.end())
            {
                return pos;
            }
        }
    }

    return Position{-1, -1}; // Нет свободных позиций
}

// Остальные методы остаются без изменений...
void GameField::PlaceCoinsRandomly(int numCoins)
{
    if (numCoins < 0)
    {
        throw std::invalid_argument("Number of coins cannot be negative");
    }

    if (numCoins > static_cast<int>(_width * _height))
    {
        throw std::invalid_argument("Too many coins for field size");
    }

    // Очищаем существующие монеты
    for (auto &row : _coins)
    {
        std::fill(row.begin(), row.end(), false);
    }

    std::uniform_int_distribution<int> distX(0, _width - 1);
    std::uniform_int_distribution<int> distY(0, _height - 1);

    int coinsPlaced = 0;
    int attempts = 0;
    const int MAX_ATTEMPTS = numCoins * 10;

    while (coinsPlaced < numCoins && attempts < MAX_ATTEMPTS)
    {
        Position pos{distX(_gen), distY(_gen)};

        // Если на позиции нет монеты и нет робота
        if (!_coins[pos.y][pos.x] && _robots.find(pos) == _robots.end())
        {
            _coins[pos.y][pos.x] = true;
            coinsPlaced++;
        }
        attempts++;
    }

    if (coinsPlaced < numCoins)
    {
        std::cout << "Warning: Could only place " << coinsPlaced
                  << " out of " << numCoins << " coins" << std::endl;
    }
    else
    {
        std::cout << "Placed " << coinsPlaced << " coins randomly on the field" << std::endl;
    }
}

bool GameField::HasCoin(const Position &pos) const
{
    return IsValidPosition(pos) && _coins[pos.y][pos.x];
}

void GameField::RemoveCoin(const Position &pos)
{
    if (!IsValidPosition(pos))
    {
        throw std::invalid_argument("Invalid position for RemoveCoin");
    }

    if (_coins[pos.y][pos.x])
    {
        _coins[pos.y][pos.x] = false;
    }
}

int GameField::GetCoinCount() const
{
    int count = 0;
    for (const auto &row : _coins)
    {
        for (bool hasCoin : row)
        {
            if (hasCoin)
            {
                count++;
            }
        }
    }
    return count;
}

bool GameField::IsValidPosition(const Position &pos) const
{
    return pos.x >= 0 && pos.y >= 0 &&
           pos.x < static_cast<int>(_width) &&
           pos.y < static_cast<int>(_height);
}

std::vector<Position> GameField::GetCoinPositions() const
{
    std::vector<Position> positions;
    for (int y = 0; y < static_cast<int>(_height); ++y)
    {
        for (int x = 0; x < static_cast<int>(_width); ++x)
        {
            if (_coins[y][x])
            {
                positions.push_back({x, y});
            }
        }
    }
    return positions;
}

void GameField::Visualize() const
{
    std::cout << "\n";

    // Заголовок с координатами X
    std::cout << "   ";
    for (size_t x = 0; x < _width; ++x)
    {
        std::cout << " " << x % 10 << " ";
    }
    std::cout << "\n";

    for (size_t y = 0; y < _height; ++y)
    {
        // Координата Y
        std::cout << (y % 10) << " │";

        for (size_t x = 0; x < _width; ++x)
        {
            Position pos{static_cast<int>(x), static_cast<int>(y)};

            char cell = '.';
            if (_robots.find(pos) != _robots.end())
            {
                cell = 'R';
            }
            else if (_coins[y][x])
            {
                cell = '$';
            }
            std::cout << " " << cell << " ";
        }
        std::cout << "│\n";
    }

    std::cout << "Legend: R - Robot, $ - Coin, . - Empty\n";
    std::cout << "Coins remaining: " << GetCoinCount() << "\n";
    std::cout << "Robots on field: " << _robots.size() << "\n";
}