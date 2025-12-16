// main.cpp
#include "Kettle.h"
#include <iostream>
#include <memory>
#include <string>
#include <limits>

void printMenu()
{
    std::cout << "\n=== Умный чайник ===\n";
    std::cout << "1. Установить стратегию нагрева\n";
    std::cout << "2. Установить целевую температуру\n";
    std::cout << "3. Проверить состояние (температура, вода)\n";
    std::cout << "4. Включить чайник\n";
    std::cout << "5. Выключить чайник\n";
    std::cout << "6. Управление уведомлениями\n";
    std::cout << "0. Выход\n";
    std::cout << "Выберите действие: ";
}

void printHeatingStrategies()
{
    std::cout << "  1. Быстрый нагрев\n";
    std::cout << "  2. Экономичный нагрев\n";
    std::cout << "  3. Точный нагрев\n";
    std::cout << "  Ваш выбор: ";
}

void printNotificationMenu()
{
    std::cout << "\n--- Управление уведомлениями ---\n";
    std::cout << "1. Подписаться на канал\n";
    std::cout << "2. Отписаться от канала\n";
    std::cout << "0. Назад\n";
    std::cout << "Выберите действие: ";
}

int main()
{
    auto notifier = std::make_shared<NotificationManager>();
    // Подписываем базовые каналы
    notifier->subscribe(std::make_shared<SoundChannel>(), Priority::LOW);
    notifier->subscribe(std::make_shared<SMSChannel>(), Priority::MEDIUM);
    notifier->subscribe(std::make_shared<EmailChannel>(), Priority::HIGH);

    Kettle kettle;
    kettle.setNotifier(notifier);
    kettle.setTargetTemp(100.0f);
    kettle.setHeatingStrategy(std::make_unique<PreciseHeating>());

    int choice;
    while (true)
    {
        printMenu();
        std::cin >> choice;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Некорректный ввод. Попробуйте снова.\n";
            continue;
        }

        switch (choice)
        {
        case 1:
        {
            int strat;
            printHeatingStrategies();
            std::cin >> strat;
            if (strat == 1)
            {
                kettle.setHeatingStrategy(std::make_unique<FastHeating>());
                std::cout << "Выбрана стратегия: Быстрый нагрев\n";
            }
            else if (strat == 2)
            {
                kettle.setHeatingStrategy(std::make_unique<EcoHeating>());
                std::cout << "Выбрана стратегия: Экономичный нагрев\n";
            }
            else if (strat == 3)
            {
                kettle.setHeatingStrategy(std::make_unique<PreciseHeating>());
                std::cout << "Выбрана стратегия: Точный нагрев\n";
            }
            else
            {
                std::cout << "Неверный выбор стратегии.\n";
            }
            break;
        }
        case 2:
        {
            float temp;
            std::cout << "Введите целевую температуру (20–100): ";
            std::cin >> temp;
            if (temp < 20 || temp > 100)
            {
                std::cout << "Температура должна быть от 20 до 100°C.\n";
            }
            else
            {
                kettle.setTargetTemp(temp);
                std::cout << "Целевая температура установлена: " << temp << "°C\n";
            }
            break;
        }
        case 3:
        {
            std::cout << "\n--- Состояние чайника ---\n";
            std::cout << "Текущая температура: " << kettle.getCurrentTemp() << "°C\n";
            std::cout << "Целевая температура: " << kettle.getTargetTemp() << "°C\n";
            std::cout << "Уровень воды: " << kettle.getWaterLevel() << " л\n";
            std::cout << "Состояние: " << (kettle.getCurrentTemp() >= 100 ? "кипяток" : kettle.getCurrentTemp() > 20 ? "нагревается"
                                                                                                                     : "холодный")
                      << "\n";
            break;
        }
        case 4:
        {
            std::cout << "Включаю чайник...\n";
            kettle.turnOn();
            break;
        }
        case 5:
        {
            std::cout << "Выключаю чайник...\n";
            kettle.turnOff();
            break;
        }
        case 6:
        {
            int notifChoice;
            while (true)
            {
                printNotificationMenu();
                std::cin >> notifChoice;
                if (notifChoice == 0)
                    break;

                if (notifChoice == 1)
                {
                    std::cout << "Доступные каналы: SMS, Email, Sound\n";
                    std::cout << "Введите название канала: ";
                    std::string name;
                    std::cin >> name;

                    std::cout << "Приоритет (1=LOW, 2=MEDIUM, 3=HIGH): ";
                    int prio;
                    std::cin >> prio;

                    Priority p = (prio == 3) ? Priority::HIGH : (prio == 2) ? Priority::MEDIUM
                                                                            : Priority::LOW;

                    if (name == "SMS")
                    {
                        notifier->subscribe(std::make_shared<SMSChannel>(), p);
                    }
                    else if (name == "Email")
                    {
                        notifier->subscribe(std::make_shared<EmailChannel>(), p);
                    }
                    else if (name == "Sound")
                    {
                        notifier->subscribe(std::make_shared<SoundChannel>(), p);
                    }
                    else
                    {
                        std::cout << "Неизвестный канал.\n";
                        continue;
                    }
                    std::cout << "Подписан на " << name << " с приоритетом " << prio << "\n";
                }
                else if (notifChoice == 2)
                {
                    std::cout << "Введите название канала для отписки (SMS, Email, Sound): ";
                    std::string name;
                    std::cin >> name;
                    notifier->unsubscribe(name);
                    std::cout << "Отписка от " << name << " выполнена.\n";
                }
                else
                {
                    std::cout << "Неверный выбор.\n";
                }
            }
            break;
        }
        case 0:
            std::cout << "Выход из программы. До свидания!\n";
            return 0;
        default:
            std::cout << "Неверный пункт меню.\n";
        }
    }

    return 0;
}