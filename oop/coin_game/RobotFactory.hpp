#pragma once
#include "Robot.hpp"
#include <memory>
#include <map>
#include <functional>
#include <stdexcept>

// pattern factory

class RobotFactory
{
public:
    using CreatorFunc = std::function<std::unique_ptr<Robot>()>;
    // создает и инициализирует робота
    static std::unique_ptr<Robot> CreateRobot(const std::string &strategyName)
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

    // возвращает все стратении от _creators
    static std::vector<std::string> GetAvailableStrategies()
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

    // Метод для регистрации новых стратегий (для расширяемости)
    static void RegisterStrategy(const std::string &name, CreatorFunc creator)
    {
        _creators[name] = creator;
    }

private:
    static std::map<std::string, CreatorFunc> _creators;

    // Регистрация стратегий при старте программы
    static bool _initialized;
    static void Initialize();
};

// Реализация в .cpp файле:
/*
std::map<std::string, RobotFactory::CreatorFunc> RobotFactory::_creators;
bool RobotFactory::_initialized = false;

void RobotFactory::Initialize() {
    _creators["random"] = []() { return std::make_unique<RandomRobot>(); };
    _creators["greedy"] = []() { return std::make_unique<GreedyRobot>(); };
    _creators["conservative"] = []() { return std::make_unique<ConservativeRobot>(); };
    _creators["explorer"] = []() { return std::make_unique<ExplorerRobot>(); };
    _creators["smart"] = []() { return std::make_unique<SmartRobot>(); };
    _initialized = true;
}
*/