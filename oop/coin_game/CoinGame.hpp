#pragma once
#include "GameEngine.hpp"
#include "GameVisualizer.hpp"
#include "TournamentManager.hpp"
#include <iostream>

class CoinCollectionGame
{
private:
    GameConfig _config;
    std::unique_ptr<GameVisualizer> _visualizer;

public:
    CoinCollectionGame(const GameConfig &config) : _config(config)
    {
        _visualizer = std::make_unique<GameVisualizer>();
    }

    void Run() // todo enum
    {
        if (_config.mode == "detailed")
            RunDetailedMode();
        else if (_config.mode == "fast")
            RunFastMode();
        else if (_config.mode == "tournament")
            RunTournamentMode();
    }

private:
    void RunDetailedMode()
    {
        GameEngine engine(_config);
        engine.Initialize();

        while (!engine.IsGameOver())
        {
            std::cout << "=== Turn " << (engine.GetCurrentTurn() + 1)
                      << " ===" << std::endl;

            engine.ExecuteTurn();
            _visualizer->DisplayField(engine.GetField());
            _visualizer->DisplayCurrentResults(engine.GetRobots());

            std::cout << "Press Enter to continue...";
            std::cin.ignore();
        }

        _visualizer->DisplayFinalResults(engine.GetRobots());
    }

    void RunFastMode()
    {
        if (_config.strategies.empty())
        {
            _config.strategies = {"random", "greedy", "conservative", "explorer"};
        }

        GameEngine engine(_config);
        engine.Initialize();

        while (!engine.IsGameOver())
        {
            engine.ExecuteTurn();
        }

        _visualizer->DisplayFinalResults(engine.GetRobots());
    }
    // либо все отдельные либо все вместе
    void RunTournamentMode()
    {
        TournamentManager tournament(_config);
        tournament.RunTournament();
        tournament.DisplayResults();
    }
};