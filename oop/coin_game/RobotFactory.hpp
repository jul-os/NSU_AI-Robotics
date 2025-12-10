#pragma once
#include "Robot.hpp"
#include <memory>
#include <map>
#include <functional>
#include <stdexcept>
#include <vector>
#include <string>

// Pattern Factory
class RobotFactory
{
public:
    using CreatorFunc = std::function<std::unique_ptr<Robot>()>;

    // Создает и инициализирует робота
    static std::unique_ptr<Robot> CreateRobot(const std::string &strategyName);

    // Возвращает все доступные стратегии
    static std::vector<std::string> GetAvailableStrategies();

    // Метод для регистрации новых стратегий (для расширяемости)
    static void RegisterStrategy(const std::string &name, CreatorFunc creator);

private:
    static std::map<std::string, CreatorFunc> _creators;
    static bool _initialized;

    // Регистрация стратегий при старте программы
    static void Initialize();
};