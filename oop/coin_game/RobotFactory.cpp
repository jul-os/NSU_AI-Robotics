#include "RobotFactory.hpp"
#include "Robot.hpp"
#include <iostream>

// Статические переменные
std::map<std::string, RobotFactory::CreatorFunc> RobotFactory::_creators;
bool RobotFactory::_initialized = false;

// Создает и инициализирует робота
std::unique_ptr<Robot> RobotFactory::CreateRobot(const std::string &strategyName)
{
    if (!_initialized)
        Initialize();

    auto it = _creators.find(strategyName);
    if (it == _creators.end())
    {
        throw std::invalid_argument("Unknown strategy: " + strategyName);
    }
    return it->second();
}

// Возвращает все доступные стратегии
std::vector<std::string> RobotFactory::GetAvailableStrategies()
{
    if (!_initialized)
        Initialize();

    std::vector<std::string> strategies;
    for (const auto &pair : _creators)
    {
        strategies.push_back(pair.first);
    }
    return strategies;
}

// Метод для регистрации новых стратегий
void RobotFactory::RegisterStrategy(const std::string &name, CreatorFunc creator)
{
    _creators[name] = creator;
}

// Регистрация всех стратегий при старте программы
void RobotFactory::Initialize()
{
    if (_initialized)
        return;

    // Регистрируем все доступные стратегии
    RegisterStrategy("RandomRobot", []() -> std::unique_ptr<Robot>
                     { return std::make_unique<RandomRobot>(); });

    RegisterStrategy("GreedyRobot", []() -> std::unique_ptr<Robot>
                     { return std::make_unique<GreedyRobot>(); });

    RegisterStrategy("CautiousRobot", []() -> std::unique_ptr<Robot>
                     { return std::make_unique<CautiousRobot>(); });

    RegisterStrategy("PatrollingRobot", []() -> std::unique_ptr<Robot>
                     { return std::make_unique<PatrollingRobot>(); });

    RegisterStrategy("SmartRobot", []() -> std::unique_ptr<Robot>
                     { return std::make_unique<SmartRobot>(); });

    RegisterStrategy("AggressiveRobot", []() -> std::unique_ptr<Robot>
                     { return std::make_unique<AggressiveRobot>(); });

    _initialized = true;
    std::cout << "RobotFactory initialized with " << _creators.size() << " strategies" << std::endl;
}