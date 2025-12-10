#include "GameEngine.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>

void GameEngine::Initialize()
{
    std::cout << "Initializing game..." << std::endl;

    // Создаем поле
    _field = std::make_unique<GameField>(_config.fieldWidth, _config.fieldHeight);
    _field->PlaceCoinsRandomly(_config.coinCount);

    // Создаем и размещаем роботов
    for (const auto &strategyName : _config.strategies)
    {
        auto robot = RobotFactory::CreateRobot(strategyName);
        if (!_field->PlaceRobot(std::move(robot)))
        {
            std::cout << "Warning: Could not place robot " << strategyName
                      << " (no free positions)" << std::endl;
        }
    }

    _currentTurn = 0;

    // Показываем стартовую информацию
    _visualizer.DisplayGameStart(_config);
    _visualizer.DisplayField(*_field, 0, _config.maxTurns);

    auto robots = _field->GetAllRobots();
    _visualizer.DisplayCurrentResults(robots, 0);

    std::cout << "Game initialized with " << robots.size() << " robots and "
              << _field->GetCoinCount() << " coins" << std::endl;
}

void GameEngine::ExecuteTurn()
{
    _currentTurn++;

    if (_config.mode == GameMode::Detailed)
    {
        std::cout << "\n"
                  << std::string(40, '=') << std::endl;
        std::cout << "TURN " << _currentTurn << "/" << _config.maxTurns << std::endl;
        std::cout << std::string(40, '=') << std::endl;
    }

    // Получаем всех роботов
    auto robots = _field->GetAllRobots();
    std::vector<std::pair<Robot *, Direction>> moves;

    // Собираем решения всех роботов
    for (auto robot : robots)
    {
        GameState state(*_field, robots, *robot, _currentTurn, _config.maxTurns);
        Direction move = robot->DecideMove(state);
        moves.push_back({robot, move});

        if (_config.mode == GameMode::Detailed)
        {
            _visualizer.DisplayRobotMove(*robot, move);
        }
    }

    // Выполняем все перемещения
    for (auto &[robot, move] : moves)
    {
        Position oldPos = robot->GetPosition();
        Position newPos = oldPos.GetNeighbor(move);

        // Создаем GameState для Move
        GameState moveState(*_field, robots, *robot, _currentTurn, _config.maxTurns);

        // Пытаемся переместить робота
        if (_field->MoveRobot(robot, newPos))
        {
            // Успешное перемещение - обновляем статистику робота
            robot->Move(move, moveState);

            // Проверяем сбор монеты на новой позиции
            if (_field->HasCoin(newPos))
            {
                robot->CollectCoin();
                _field->RemoveCoin(newPos);
                std::cout << "🎉 " << robot->GetName() << " collected a coin at ("
                          << newPos.x << ", " << newPos.y << ")!" << std::endl;
            }
        }
        else
        {
            // Если перемещение не удалось, робот остается на месте
            std::cout << "❌ " << robot->GetName() << " could not move to ("
                      << newPos.x << ", " << newPos.y << "). Staying put." << std::endl;
            // Все равно вызываем Move с STAY для обновления статистики
            robot->Move(Direction::STAY, moveState);
        }
    }

    // Показываем результаты после хода
    if (_config.mode == GameMode::Detailed)
    {
        _visualizer.DisplayField(*_field, _currentTurn, _config.maxTurns);

        auto currentRobots = _field->GetAllRobots();
        _visualizer.DisplayCurrentResults(currentRobots, _currentTurn);

        // Ждем ввода пользователя для продолжения
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore();
    }
    else if (_config.mode == GameMode::Fast && _currentTurn % 10 == 0)
    {
        std::cout << "Turn " << _currentTurn << "/" << _config.maxTurns
                  << " - Coins remaining: " << _field->GetCoinCount() << std::endl;
    }
}
void GameEngine::RunGame()
{
    Initialize();

    while (!IsGameOver())
    {
        ExecuteTurn();

        if (_field->GetCoinCount() == 0)
        {
            if (_config.mode == GameMode::Detailed || _config.mode == GameMode::Fast)
            {
                std::cout << "\nAll coins have been collected! Game ended early." << std::endl;
            }
            break;
        }

        if (_config.mode == GameMode::Fast)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    // Показываем финальные результаты
    _visualizer.DisplayGameEnd();

    auto finalRobots = _field->GetAllRobots();
    _visualizer.DisplayFinalResults(finalRobots);
}

bool GameEngine::IsGameOver() const
{
    return _currentTurn >= _config.maxTurns;
}

// Удаляем старый метод UpdateRobotPosition, так как он больше не нужен