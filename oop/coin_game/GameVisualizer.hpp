#pragma once
#include "GameField.hpp"
#include "Robot.hpp"
#include <iostream>

class GameVisualizer
{
public:
    // отобразить поле
    void DisplayField(const GameField &field) const
    {
        // Красиво отображаем поле с роботами и монетами
        field.Visualize();
    }
    // отобразить текущий результат
    void DisplayCurrentResults(const std::vector<std::unique_ptr<Robot>> &robots) const
    {
        std::cout << "Current results:\n";
        for (const auto &robot : robots)
        {
            std::cout << robot->GetName() << ": " << robot->GetCoinsCollected()
                      << " coins\n";
        }
    }

    // отобразить результат в конце: подробная статистика с учетом эффективности
    void DisplayFinalResults(const std::vector<std::unique_ptr<Robot>> &robots) const;
};