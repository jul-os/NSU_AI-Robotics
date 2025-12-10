#include "CoinGame.hpp"
#include "RobotFactory.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

// Простой парсер аргументов командной строки
class CommandLineParser
{
private:
    std::vector<std::string> _args;

public:
    CommandLineParser(int argc, char *argv[])
    {
        for (int i = 1; i < argc; ++i)
        {
            _args.push_back(argv[i]);
        }
    }

    bool HasOption(const std::string &option) const
    {
        return std::find(_args.begin(), _args.end(), option) != _args.end();
    }

    std::string GetOptionValue(const std::string &option, const std::string &defaultValue = "") const
    {
        auto it = std::find(_args.begin(), _args.end(), option);
        if (it != _args.end() && ++it != _args.end())
        {
            return *it;
        }
        return defaultValue;
    }

    std::vector<std::string> GetStrategies() const
    {
        std::vector<std::string> strategies;

        // Ищем аргумент --strategies
        auto it = std::find(_args.begin(), _args.end(), "--strategies");
        if (it != _args.end() && ++it != _args.end())
        {
            std::string strategiesStr = *it;
            std::stringstream ss(strategiesStr);
            std::string strategy;

            while (std::getline(ss, strategy, ','))
            {
                // Убираем пробелы
                strategy.erase(0, strategy.find_first_not_of(' '));
                strategy.erase(strategy.find_last_not_of(' ') + 1);
                if (!strategy.empty())
                {
                    strategies.push_back(strategy);
                }
            }
        }

        return strategies;
    }

    void ShowHelp() const
    {
        std::cout << "Robot Coin Collection Game\n";
        std::cout << "Usage:\n";
        std::cout << "  --mode <mode>           Game mode: detailed, fast, tournament\n";
        std::cout << "  --strategies <list>     Comma-separated list of strategies\n";
        std::cout << "  --width <number>        Field width (default: 10)\n";
        std::cout << "  --height <number>       Field height (default: 8)\n";
        std::cout << "  --coins <number>        Number of coins (default: 15)\n";
        std::cout << "  --turns <number>        Maximum turns (default: 50)\n";
        std::cout << "  --games <number>        Number of games (tournament mode, default: 1)\n";
        std::cout << "  --help                  Show this help message\n";
        std::cout << "\nAvailable strategies:\n";

        auto available = RobotFactory::GetAvailableStrategies();
        for (const auto &strategy : available)
        {
            std::cout << "  " << strategy << "\n";
        }

        std::cout << "\nExamples:\n";
        std::cout << "  ./robot_game --mode detailed --strategies \"GreedyRobot,RandomRobot\" --width 8 --height 6 --coins 10 --turns 20\n";
        std::cout << "  ./robot_game --mode fast --strategies \"GreedyRobot,RandomRobot,SmartRobot,CautiousRobot\"\n";
        std::cout << "  ../robot_game --mode tournament --strategies \"GreedyRobot,RandomRobot,SmartRobot,CautiousRobot,PatrollingRobot\" --games 10\n";
    }
};

GameMode ParseGameMode(const std::string &modeStr)
{
    if (modeStr == "detailed")
        return GameMode::Detailed;
    if (modeStr == "fast")
        return GameMode::Fast;
    if (modeStr == "tournament")
        return GameMode::Tournament;

    std::cout << "Unknown mode: " << modeStr << ". Using 'fast' mode.\n";
    return GameMode::Fast;
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        // Если нет аргументов, показываем помощь
        CommandLineParser parser(0, nullptr);
        parser.ShowHelp();
        return 0;
    }

    CommandLineParser parser(argc, argv);

    if (parser.HasOption("--help"))
    {
        parser.ShowHelp();
        return 0;
    }

    try
    {
        GameConfig config;

        // Парсим режим игры
        std::string modeStr = parser.GetOptionValue("--mode", "fast");
        config.mode = ParseGameMode(modeStr);

        // Парсим стратегии
        config.strategies = parser.GetStrategies();
        if (config.strategies.empty())
        {
            // Если стратегии не указаны, используем все доступные
            config.strategies = RobotFactory::GetAvailableStrategies();
            std::cout << "No strategies specified. Using all available: ";
            for (const auto &s : config.strategies)
            {
                std::cout << s << " ";
            }
            std::cout << "\n";
        }

        // Валидация стратегий
        auto available = RobotFactory::GetAvailableStrategies();
        for (const auto &strategy : config.strategies)
        {
            if (std::find(available.begin(), available.end(), strategy) == available.end())
            {
                throw std::invalid_argument("Unknown strategy: " + strategy);
            }
        }

        // Парсим параметры поля
        config.fieldWidth = std::stoi(parser.GetOptionValue("--width", "10"));
        config.fieldHeight = std::stoi(parser.GetOptionValue("--height", "8"));
        config.coinCount = std::stoi(parser.GetOptionValue("--coins", "15"));
        config.maxTurns = std::stoi(parser.GetOptionValue("--turns", "50"));
        config.gameCount = std::stoi(parser.GetOptionValue("--games", "1"));

        // Валидация параметров
        if (config.fieldWidth <= 0 || config.fieldHeight <= 0)
        {
            throw std::invalid_argument("Field dimensions must be positive");
        }
        if (config.coinCount <= 0)
        {
            throw std::invalid_argument("Coin count must be positive");
        }
        if (config.maxTurns <= 0)
        {
            throw std::invalid_argument("Turn count must be positive");
        }
        if (config.gameCount <= 0)
        {
            throw std::invalid_argument("Game count must be positive");
        }

        // Выводим информацию о конфигурации
        std::cout << "=== Game Configuration ===\n";
        std::cout << "Mode: " << modeStr << "\n";
        std::cout << "Field: " << config.fieldWidth << "x" << config.fieldHeight << "\n";
        std::cout << "Coins: " << config.coinCount << "\n";
        std::cout << "Max turns: " << config.maxTurns << "\n";
        std::cout << "Games: " << config.gameCount << "\n";
        std::cout << "Strategies: ";
        for (size_t i = 0; i < config.strategies.size(); ++i)
        {
            std::cout << config.strategies[i];
            if (i < config.strategies.size() - 1)
                std::cout << ", ";
        }
        std::cout << "\n==========================\n\n";

        // Запускаем игру/игры
        if (config.mode != GameMode::Tournament)
        {
            // Для detailed и fast режимов запускаем указанное количество игр
            for (int game = 1; game <= config.gameCount; ++game)
            {
                if (config.gameCount > 1)
                {
                    std::cout << "=== Game " << game << "/" << config.gameCount << " ===\n";
                }

                CoinCollectionGame gameInstance(config);
                gameInstance.Run();

                if (game < config.gameCount)
                {
                    std::cout << "\n"
                              << std::string(50, '-') << "\n\n";
                }
            }
        }
        else
        {
            // Турнирный режим
            CoinCollectionGame tournamentGame(config);
            tournamentGame.Run();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << "Use --help for usage information.\n";
        return 1;
    }

    return 0;
}