#include "GameVisualizer.hpp"
#include "Robot.hpp"
#include <algorithm>
#include <iomanip>
#include <limits>

void GameVisualizer::DisplayFinalResults(const std::vector<Robot *> &robots) const
{
    std::cout << "\n"
              << std::string(60, '=') << "\n";
    std::cout << "FINAL RESULTS\n";
    std::cout << std::string(60, '=') << "\n";

    // Создаем копию для сортировки (уже vector<Robot*>)
    std::vector<Robot *> robotPtrs = robots;

    // Сортируем по правилам соревнования:
    // 1. Количество монет (по убыванию)
    // 2. При равенстве - эффективность (меньше ходов на сбор лучше)
    // 3. При равенстве эффективности - меньшее общее расстояние
    std::sort(robotPtrs.begin(), robotPtrs.end(),
              [](const Robot *a, const Robot *b)
              {
                  // Сначала по монетам
                  if (a->GetCoinsCollected() != b->GetCoinsCollected())
                  {
                      return a->GetCoinsCollected() > b->GetCoinsCollected();
                  }

                  // При равенстве монет - по эффективности (ходы на монету)
                  double efficiencyA = (a->GetCoinsCollected() > 0) ? static_cast<double>(a->GetMovesMade()) / a->GetCoinsCollected() : std::numeric_limits<double>::max();

                  double efficiencyB = (b->GetCoinsCollected() > 0) ? static_cast<double>(b->GetMovesMade()) / b->GetCoinsCollected() : std::numeric_limits<double>::max();

                  if (std::abs(efficiencyA - efficiencyB) > 1e-6)
                  {
                      return efficiencyA < efficiencyB;
                  }

                  // При равенстве эффективности - по общему расстоянию
                  return a->GetTotalDistance() < b->GetTotalDistance();
              });

    // Заголовок таблицы
    std::cout << std::left
              << std::setw(4) << "Pos"
              << std::setw(20) << "Robot Name"
              << std::setw(8) << "Coins"
              << std::setw(12) << "Moves"
              << std::setw(10) << "Distance"
              << std::setw(15) << "Efficiency"
              << "\n";
    std::cout << std::string(70, '-') << "\n";

    // Выводим результаты
    for (size_t i = 0; i < robotPtrs.size(); ++i)
    {
        const Robot *robot = robotPtrs[i];

        // Рассчитываем эффективность (ходы на одну монету)
        std::string efficiency;
        if (robot->GetCoinsCollected() > 0)
        {
            double eff = static_cast<double>(robot->GetMovesMade()) / robot->GetCoinsCollected();
            efficiency = std::to_string(eff);
            // Обрезаем до 2 знаков после запятой
            efficiency = efficiency.substr(0, efficiency.find('.') + 3);
        }
        else
        {
            efficiency = "N/A";
        }

        std::cout << std::left
                  << std::setw(4) << (i + 1)
                  << std::setw(20) << robot->GetName()
                  << std::setw(8) << robot->GetCoinsCollected()
                  << std::setw(12) << robot->GetMovesMade()
                  << std::setw(10) << robot->GetTotalDistance()
                  << std::setw(15) << efficiency
                  << "\n";
    }

    // Определяем победителя
    if (!robotPtrs.empty())
    {
        std::cout << "\n🏆 WINNER: " << robotPtrs[0]->GetName()
                  << " with " << robotPtrs[0]->GetCoinsCollected() << " coins! 🏆\n";
    }

    // Дополнительная статистика
    std::cout << "\n"
              << std::string(40, '-') << "\n";
    std::cout << "ADDITIONAL STATISTICS:\n";
    std::cout << std::string(40, '-') << "\n";

    int totalCoins = 0;
    int totalMoves = 0;
    for (const auto &robot : robotPtrs)
    {
        totalCoins += robot->GetCoinsCollected();
        totalMoves += robot->GetMovesMade();
    }

    std::cout << "Total coins collected by all robots: " << totalCoins << "\n";
    std::cout << "Total moves made by all robots: " << totalMoves << "\n";

    if (totalCoins > 0)
    {
        double avgEfficiency = static_cast<double>(totalMoves) / totalCoins;
        std::cout << "Average efficiency (moves per coin): "
                  << std::fixed << std::setprecision(2) << avgEfficiency << "\n";
    }
}

void GameVisualizer::DisplayField(const GameField &field, int currentTurn, int totalTurns) const
{
    if (currentTurn >= 0 && totalTurns >= 0)
    {
        std::cout << "\n=== Turn " << currentTurn << "/" << totalTurns << " ===";
    }
    std::cout << "\n";
    field.Visualize();
}

void GameVisualizer::DisplayCurrentResults(const std::vector<Robot *> &robots, int currentTurn) const
{
    if (currentTurn >= 0)
    {
        std::cout << "\n--- Results after turn " << currentTurn << " ---\n";
    }
    else
    {
        std::cout << "\n--- Current Results ---\n";
    }

    for (const auto &robot : robots)
    {
        std::cout << robot->GetName() << ": " << robot->GetCoinsCollected()
                  << " coins (moves: " << robot->GetMovesMade()
                  << ", distance: " << robot->GetTotalDistance() << ")\n";
    }
}

void GameVisualizer::DisplayRobotMove(const Robot &robot, Direction move) const
{
    std::cout << "🤖 " << robot.GetName() << " decides to move: "
              << DirectionToString(move) << "\n";
}

void GameVisualizer::DisplayGameStart(const GameConfig &config) const
{
    std::cout << "\n"
              << std::string(50, '=') << "\n";
    std::cout << "🎮 ROBOT COIN COLLECTION COMPETITION STARTING! 🎮\n";
    std::cout << std::string(50, '=') << "\n";
    std::cout << "Field: " << config.fieldWidth << "x" << config.fieldHeight << "\n";
    std::cout << "Coins: " << config.coinCount << "\n";
    std::cout << "Max turns: " << config.maxTurns << "\n";
    std::cout << "Strategies: ";
    for (size_t i = 0; i < config.strategies.size(); ++i)
    {
        std::cout << config.strategies[i];
        if (i < config.strategies.size() - 1)
            std::cout << ", ";
    }
    std::cout << "\n"
              << std::string(50, '=') << "\n";
}

void GameVisualizer::DisplayGameEnd() const
{
    std::cout << "\n"
              << std::string(50, '=') << "\n";
    std::cout << "🎯 GAME OVER! 🎯\n";
    std::cout << std::string(50, '=') << "\n";
}

std::string GameVisualizer::DirectionToString(Direction dir) const
{
    switch (dir)
    {
    case Direction::UP:
        return "UP ↑";
    case Direction::DOWN:
        return "DOWN ↓";
    case Direction::LEFT:
        return "LEFT ←";
    case Direction::RIGHT:
        return "RIGHT →";
    case Direction::STAY:
        return "STAY ●";
    default:
        return "UNKNOWN";
    }
}