#pragma once
#include "GameEngine.hpp"
#include "GameVisualizer.hpp"
#include "RobotFactory.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <limits>

class CoinCollectionGame
{
private:
    GameConfig _config;
    std::unique_ptr<GameVisualizer> _visualizer;

public:
    // конструктор
    CoinCollectionGame(const GameConfig &config) : _config(config)
    {
        _visualizer = std::make_unique<GameVisualizer>();
    }

    // запускает игру
    void Run()
    {
        switch (_config.mode)
        {
        case GameMode::Detailed:
            RunDetailedMode();
            break;
        case GameMode::Fast:
            RunFastMode();
            break;
        case GameMode::Tournament:
            RunTournamentMode();
            break;
        }
    }

private:
    void RunDetailedMode()
    {
        GameEngine engine(_config);
        engine.RunGame(); // Используем готовый метод RunGame из GameEngine
    }

    void RunFastMode()
    {
        // Если стратегии не указаны, используем 4 стандартные
        if (_config.strategies.empty())
        {
            auto allStrategies = RobotFactory::GetAvailableStrategies();
            // Берем первые 4 стратегии или все, если меньше 4
            _config.strategies.assign(
                allStrategies.begin(),
                allStrategies.begin() + std::min<size_t>(4, allStrategies.size()));
        }

        // Ограничиваем до 4 стратегий в быстром режиме
        if (_config.strategies.size() > 4)
        {
            std::cout << "Fast mode: using first 4 strategies from provided list\n";
            _config.strategies.resize(4);
        }

        std::cout << "Fast Mode: " << _config.strategies.size()
                  << " strategies, " << _config.maxTurns << " turns, "
                  << _config.coinCount << " coins\n";

        GameEngine engine(_config);
        engine.RunGame();
    }

    void RunTournamentMode()
    {
        if (_config.strategies.size() < 5)
        {
            std::cout << "Tournament mode requires at least 5 strategies.\n";
            std::cout << "Available strategies: ";
            auto available = RobotFactory::GetAvailableStrategies();
            for (const auto &strategy : available)
            {
                std::cout << strategy << " ";
            }
            std::cout << "\n";
            return;
        }

        // Для турнира используем все переданные стратегии
        std::vector<std::string> tournamentStrategies = _config.strategies;
        int numStrategies = tournamentStrategies.size();

        // Количество игр: C(n, 4) - сочетания из n по 4
        int totalGames = CalculateCombinations(numStrategies, 4);

        std::cout << "\n"
                  << std::string(60, '=') << "\n";
        std::cout << "🏆 TOURNAMENT MODE 🏆\n";
        std::cout << std::string(60, '=') << "\n";
        std::cout << "Strategies: " << numStrategies << "\n";
        std::cout << "Total games: " << totalGames << "\n";
        std::cout << "Field: " << _config.fieldWidth << "x" << _config.fieldHeight << "\n";
        std::cout << "Coins per game: " << _config.coinCount << "\n";
        std::cout << "Turns per game: " << _config.maxTurns << "\n";
        std::cout << std::string(60, '=') << "\n\n";

        // Статистика турнира
        std::map<std::string, TournamentStats> tournamentStats;

        // Генерируем все комбинации по 4 стратегии
        int gameNumber = 1;
        std::vector<int> combination(4);
        for (int i = 0; i < 4; ++i)
        {
            combination[i] = i;
        }

        do
        {
            // Выбираем стратегии для этой игры
            std::vector<std::string> gameStrategies;
            for (int idx : combination)
            {
                gameStrategies.push_back(tournamentStrategies[idx]);
            }

            std::cout << "🎮 Game " << gameNumber << "/" << totalGames << ": ";
            for (const auto &strategy : gameStrategies)
            {
                std::cout << strategy << " ";
            }
            std::cout << "\n";

            // Запускаем игру
            GameConfig gameConfig = _config;
            gameConfig.strategies = gameStrategies;
            gameConfig.mode = GameMode::Fast; // Турнирные игры в быстром режиме

            GameEngine engine(gameConfig);
            engine.RunGame();

            // Собираем статистику - теперь передаем vector<Robot*>
            UpdateTournamentStats(engine.GetRobots(), tournamentStats);

            gameNumber++;

        } while (NextCombination(combination, numStrategies));

        // Выводим итоги турнира
        DisplayTournamentResults(tournamentStats);
    }

private:
    struct TournamentStats
    {
        int totalGames = 0;
        int totalWins = 0;
        int totalCoins = 0;
        int totalSecondPlace = 0;
        int totalThirdPlace = 0;
    };

    // Расчет сочетаний C(n, k)
    int CalculateCombinations(int n, int k) const
    {
        if (k > n)
            return 0;
        if (k * 2 > n)
            k = n - k;

        int result = 1;
        for (int i = 1; i <= k; ++i)
        {
            result *= (n - i + 1);
            result /= i;
        }
        return result;
    }

    // Генерация следующей комбинации
    bool NextCombination(std::vector<int> &combination, int n)
    {
        int k = combination.size();
        for (int i = k - 1; i >= 0; --i)
        {
            if (combination[i] < n - k + i)
            {
                combination[i]++;
                for (int j = i + 1; j < k; ++j)
                {
                    combination[j] = combination[j - 1] + 1;
                }
                return true;
            }
        }
        return false;
    }

    // Обновление статистики турнира - ИЗМЕНЕНО: принимает vector<Robot*>
    void UpdateTournamentStats(const std::vector<Robot *> &robots,
                               std::map<std::string, TournamentStats> &stats)
    {
        // Сортируем роботов по количеству монет
        std::vector<const Robot *> sortedRobots;
        for (const auto robot : robots) // Прямой доступ к указателям
        {
            sortedRobots.push_back(robot);
        }

        std::sort(sortedRobots.begin(), sortedRobots.end(),
                  [](const Robot *a, const Robot *b)
                  {
                      return a->GetCoinsCollected() > b->GetCoinsCollected();
                  });

        // Обновляем статистику для каждого робота
        for (size_t i = 0; i < sortedRobots.size(); ++i)
        {
            const std::string &name = sortedRobots[i]->GetName();
            stats[name].totalGames++;
            stats[name].totalCoins += sortedRobots[i]->GetCoinsCollected();

            if (i == 0)
                stats[name].totalWins++;
            if (i == 1)
                stats[name].totalSecondPlace++;
            if (i == 2)
                stats[name].totalThirdPlace++;
        }
    }

    // Вывод результатов турнира
    void DisplayTournamentResults(const std::map<std::string, TournamentStats> &stats) const
    {
        std::cout << "\n"
                  << std::string(70, '=') << "\n";
        std::cout << "🎉 TOURNAMENT FINAL RESULTS 🎉\n";
        std::cout << std::string(70, '=') << "\n";

        // Преобразуем в вектор для сортировки
        std::vector<std::pair<std::string, TournamentStats>> sortedStats;
        for (const auto &pair : stats)
        {
            sortedStats.push_back(pair);
        }

        // Сортируем по победам, затем по общему количеству монет
        std::sort(sortedStats.begin(), sortedStats.end(),
                  [](const auto &a, const auto &b)
                  {
                      if (a.second.totalWins != b.second.totalWins)
                      {
                          return a.second.totalWins > b.second.totalWins;
                      }
                      if (a.second.totalSecondPlace != b.second.totalSecondPlace)
                      {
                          return a.second.totalSecondPlace > b.second.totalSecondPlace;
                      }
                      return a.second.totalCoins > b.second.totalCoins;
                  });

        // Выводим таблицу результатов
        std::cout << std::left
                  << std::setw(4) << "Pos"
                  << std::setw(20) << "Strategy"
                  << std::setw(8) << "Games"
                  << std::setw(8) << "Wins"
                  << std::setw(8) << "2nd"
                  << std::setw(8) << "3rd"
                  << std::setw(12) << "Total Coins"
                  << std::setw(15) << "Avg Coins/Game"
                  << "\n";
        std::cout << std::string(80, '-') << "\n";

        for (size_t i = 0; i < sortedStats.size(); ++i)
        {
            const auto &[name, stat] = sortedStats[i];
            double avgCoins = static_cast<double>(stat.totalCoins) / stat.totalGames;

            std::cout << std::left
                      << std::setw(4) << (i + 1)
                      << std::setw(20) << name
                      << std::setw(8) << stat.totalGames
                      << std::setw(8) << stat.totalWins
                      << std::setw(8) << stat.totalSecondPlace
                      << std::setw(8) << stat.totalThirdPlace
                      << std::setw(12) << stat.totalCoins
                      << std::setw(15) << std::fixed << std::setprecision(2) << avgCoins
                      << "\n";
        }

        // Определяем победителя турнира
        if (!sortedStats.empty())
        {
            std::cout << "\n🏆 TOURNAMENT CHAMPION: " << sortedStats[0].first
                      << " with " << sortedStats[0].second.totalWins << " wins! 🏆\n";
        }

        std::cout << std::string(70, '=') << "\n";
    }
};