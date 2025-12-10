#include <iostream>
#include "Robot.hpp"
#include "GameField.hpp"
#include <random>
#include <algorithm>
#include <vector>
#include <limits>

void Robot::Move(Direction dir, GameState &state)
{
    Position newPos = _position.GetNeighbor(dir);

    // Проверяем валидность новой позиции
    if (!state.field.IsValidPosition(newPos))
    {
        std::cout << _name << " tried to move to invalid position ("
                  << newPos.x << ", " << newPos.y << "), staying put." << std::endl;
        return;
    }

    // Обновляем статистику перемещений
    _movesMade++;
    _totalDistance += (dir != Direction::STAY) ? 1 : 0;

    // Устанавливаем новую позицию
    _position = newPos;

    // Если есть монетка на этой клетке, собираем ее
    if (state.field.HasCoin(newPos))
    {
        CollectCoin();
        state.field.RemoveCoin(newPos);
        std::cout << _name << " collected a coin at (" << newPos.x << ", " << newPos.y << ")" << std::endl;
    }

    // Минимальный лог
    std::cout << _name << " moved to (" << newPos.x << ", " << newPos.y << ")" << std::endl;
}

// RandomRobot - случайные перемещения
Direction RandomRobot::DecideMove(const GameState &state)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dist(0, 4); // 0-4 для 5 направлений

    return static_cast<Direction>(dist(gen));
}

// GreedyRobot - жадный сбор ближайших монет
Direction GreedyRobot::DecideMove(const GameState &state)
{
    const auto &field = state.field;
    Position currentPos = _position;

    // Получаем все позиции с монетами
    auto coinPositions = field.GetCoinPositions();

    if (coinPositions.empty())
    {
        // Если монет нет, остаемся на месте
        return Direction::STAY;
    }

    // Находим ближайшую монету
    Position nearestCoin = coinPositions[0];
    int minDistance = currentPos.DistanceTo(nearestCoin);

    for (size_t i = 1; i < coinPositions.size(); ++i)
    {
        int distance = currentPos.DistanceTo(coinPositions[i]);
        if (distance < minDistance)
        {
            minDistance = distance;
            nearestCoin = coinPositions[i];
        }
    }

    // Выбираем направление к ближайшей монете
    int dx = nearestCoin.x - currentPos.x;
    int dy = nearestCoin.y - currentPos.y;

    // Предпочитаем движение по оси с большей разницей
    if (std::abs(dx) > std::abs(dy))
    {
        return (dx > 0) ? Direction::RIGHT : Direction::LEFT;
    }
    else if (std::abs(dy) > std::abs(dx))
    {
        return (dy > 0) ? Direction::DOWN : Direction::UP;
    }
    else
    {
        // Если разницы равны, выбираем случайно
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<int> dist(0, 1);

        if (dx != 0)
        {
            return (dx > 0) ? Direction::RIGHT : Direction::LEFT;
        }
        else
        {
            return (dy > 0) ? Direction::DOWN : Direction::UP;
        }
    }
}

// CautiousRobot - избегает столкновений с другими роботами
Direction CautiousRobot::DecideMove(const GameState &state)
{
    const auto &field = state.field;
    Position currentPos = _position;

    // Получаем позиции всех роботов
    auto robotPositions = field.GetRobotPositions();

    // Находим ближайшего робота (исключая себя)
    int minDistance = std::numeric_limits<int>::max();
    Position nearestRobot = currentPos;

    for (const auto &pos : robotPositions)
    {
        if (pos == currentPos)
            continue; // Пропускаем себя

        int distance = currentPos.DistanceTo(pos);
        if (distance < minDistance)
        {
            minDistance = distance;
            nearestRobot = pos;
        }
    }

    // Если рядом есть роботы, отдаляемся от ближайшего
    if (minDistance <= 2 && minDistance > 0)
    {
        int dx = currentPos.x - nearestRobot.x;
        int dy = currentPos.y - nearestRobot.y;

        // Двигаемся в направлении, противоположном ближайшему роботу
        if (std::abs(dx) > std::abs(dy))
        {
            return (dx > 0) ? Direction::RIGHT : Direction::LEFT;
        }
        else
        {
            return (dy > 0) ? Direction::DOWN : Direction::UP;
        }
    }

    // Иначе ведем себя как жадный робот
    auto coinPositions = field.GetCoinPositions();
    if (coinPositions.empty())
    {
        return Direction::STAY;
    }

    // Находим ближайшую монету
    Position nearestCoin = coinPositions[0];
    int coinDistance = currentPos.DistanceTo(nearestCoin);

    for (size_t i = 1; i < coinPositions.size(); ++i)
    {
        int distance = currentPos.DistanceTo(coinPositions[i]);
        if (distance < coinDistance)
        {
            coinDistance = distance;
            nearestCoin = coinPositions[i];
        }
    }

    // Двигаемся к монете
    int dx = nearestCoin.x - currentPos.x;
    int dy = nearestCoin.y - currentPos.y;

    if (std::abs(dx) > std::abs(dy))
    {
        return (dx > 0) ? Direction::RIGHT : Direction::LEFT;
    }
    else
    {
        return (dy > 0) ? Direction::DOWN : Direction::UP;
    }
}

// PatrollingRobot - движется по патрульному маршруту
Direction PatrollingRobot::DecideMove(const GameState &state)
{
    const auto &field = state.field;
    Position currentPos = _position;

    // Проверяем, есть ли монеты рядом
    std::vector<Direction> directions = {
        Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};

    for (Direction dir : directions)
    {
        Position neighbor = currentPos.GetNeighbor(dir);
        if (field.IsValidPosition(neighbor) && field.HasCoin(neighbor))
        {
            return dir; // Идем к монете, если она рядом
        }
    }

    // Патрулирование: двигаемся по границам поля
    if (_movingRight)
    {
        if (currentPos.x < static_cast<int>(field.GetWidth()) - 2)
        {
            return Direction::RIGHT;
        }
        else
        {
            _movingRight = false;
            return Direction::DOWN;
        }
    }
    else
    {
        if (currentPos.x > 1)
        {
            return Direction::LEFT;
        }
        else
        {
            _movingRight = true;
            return Direction::DOWN;
        }
    }
}

// SmartRobot - комбинированная стратегия
Direction SmartRobot::DecideMove(const GameState &state)
{
    const auto &field = state.field;
    Position currentPos = _position;

    // Если осталось мало ходов, становимся более агрессивными
    int turnsLeft = state.totalTurns - state.currentTurn;
    bool aggressiveMode = (turnsLeft < 10);

    // Ищем ближайшую монету
    auto coinPositions = field.GetCoinPositions();
    if (coinPositions.empty())
    {
        return Direction::STAY;
    }

    // Находим несколько ближайших монет и выбираем лучшую
    struct CoinTarget
    {
        Position pos;
        int distance;
        int otherRobotsNearby;
    };

    std::vector<CoinTarget> targets;
    for (const auto &coinPos : coinPositions)
    {
        int distance = currentPos.DistanceTo(coinPos);
        int robotsNearby = 0;

        // Считаем роботов рядом с монетой
        for (const auto robot : state.allRobots)
        {
            // Сравниваем по имени, так как указатели могут быть разными
            if (robot->GetName() == this->GetName())
                continue;
            if (coinPos.DistanceTo(robot->GetPosition()) <= 2)
            {
                robotsNearby++;
            }
        }

        targets.push_back({coinPos, distance, robotsNearby});
    }

    // Сортируем монеты по эффективности (расстояние + конкуренция)
    std::sort(targets.begin(), targets.end(),
              [aggressiveMode](const CoinTarget &a, const CoinTarget &b)
              {
                  int scoreA = a.distance + (aggressiveMode ? a.otherRobotsNearby * 2 : a.otherRobotsNearby * 5);
                  int scoreB = b.distance + (aggressiveMode ? b.otherRobotsNearby * 2 : b.otherRobotsNearby * 5);
                  return scoreA < scoreB;
              });

    Position bestCoin = targets[0].pos;

    // Двигаемся к лучшей монете
    int dx = bestCoin.x - currentPos.x;
    int dy = bestCoin.y - currentPos.y;

    if (std::abs(dx) > std::abs(dy))
    {
        return (dx > 0) ? Direction::RIGHT : Direction::LEFT;
    }
    else
    {
        return (dy > 0) ? Direction::DOWN : Direction::UP;
    }
}

// AggressiveRobot - пытается мешать другим роботам
Direction AggressiveRobot::DecideMove(const GameState &state)
{
    const auto &field = state.field;
    Position currentPos = _position;

    // Ищем робота с наибольшим количеством монет
    const Robot *richestRobot = nullptr;
    int maxCoins = -1;

    for (const auto robot : state.allRobots)
    {
        // Сравниваем по имени вместо указателя
        if (robot->GetName() == this->GetName())
            continue;
        if (robot->GetCoinsCollected() > maxCoins)
        {
            maxCoins = robot->GetCoinsCollected();
            richestRobot = robot;
        }
    }

    // Если нашли богатого робота, пытаемся ему помешать
    if (richestRobot && maxCoins > _coinsCollected)
    {
        Position targetPos = richestRobot->GetPosition();
        int dx = targetPos.x - currentPos.x;
        int dy = targetPos.y - currentPos.y;

        // Двигаемся к цели
        if (std::abs(dx) > std::abs(dy))
        {
            return (dx > 0) ? Direction::RIGHT : Direction::LEFT;
        }
        else
        {
            return (dy > 0) ? Direction::DOWN : Direction::UP;
        }
    }

    // Иначе собираем монеты как жадный робот
    auto coinPositions = field.GetCoinPositions();
    if (coinPositions.empty())
    {
        return Direction::STAY;
    }

    Position nearestCoin = coinPositions[0];
    int minDistance = currentPos.DistanceTo(nearestCoin);

    for (size_t i = 1; i < coinPositions.size(); ++i)
    {
        int distance = currentPos.DistanceTo(coinPositions[i]);
        if (distance < minDistance)
        {
            minDistance = distance;
            nearestCoin = coinPositions[i];
        }
    }

    int dx = nearestCoin.x - currentPos.x;
    int dy = nearestCoin.y - currentPos.y;

    if (std::abs(dx) > std::abs(dy))
    {
        return (dx > 0) ? Direction::RIGHT : Direction::LEFT;
    }
    else
    {
        return (dy > 0) ? Direction::DOWN : Direction::UP;
    }
}