// tests/kettle_tests.cpp
#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "NotificationChannel.h"
#include "NotificationManager.h"
#include "Kettle.h"

// === Mock-канал для тестирования ===
class MockChannel : public NotificationChannel
{
public:
    std::vector<std::string> messages;

    void send(const std::string &message) override
    {
        messages.push_back(message);
    }

    std::string getName() const override
    {
        return "Mock";
    }

    bool hasMessage(const std::string &substr) const
    {
        for (const auto &msg : messages)
        {
            if (msg.find(substr) != std::string::npos)
                return true;
        }
        return false;
    }
};

// === Тесты стратегий ===
TEST(HeatingStrategyTest, FastHeating_IncreasesTempBy3AndDecreasesWater)
{
    Kettle kettle;
    float initialTemp = kettle.getCurrentTemp();
    float initialWater = kettle.getWaterLevel();

    FastHeating strategy;
    strategy.heat(kettle);

    EXPECT_NEAR(kettle.getCurrentTemp(), initialTemp + 3.0f, 1e-5f);
    EXPECT_NEAR(kettle.getWaterLevel(), initialWater - 0.02f, 1e-5f);
}

TEST(HeatingStrategyTest, EcoHeating_IncreasesTempBy1AndDecreasesWaterLess)
{
    Kettle kettle;
    float initialTemp = kettle.getCurrentTemp();
    float initialWater = kettle.getWaterLevel();

    EcoHeating strategy;
    strategy.heat(kettle);

    EXPECT_NEAR(kettle.getCurrentTemp(), initialTemp + 1.0f, 1e-5f);
    EXPECT_NEAR(kettle.getWaterLevel(), initialWater - 0.005f, 1e-5f);
}

TEST(HeatingStrategyTest, PreciseHeating_UsesLargeStepWhenFarFromTarget)
{
    Kettle kettle;
    kettle.setTargetTemp(100.0f); // далеко от 20°C
    float before = kettle.getCurrentTemp();

    PreciseHeating strategy;
    strategy.heat(kettle);

    EXPECT_NEAR(kettle.getCurrentTemp(), before + 2.0f, 1e-5f);
}

TEST(HeatingStrategyTest, PreciseHeating_UsesSmallStepWhenCloseToTarget)
{
    Kettle kettle;
    kettle.setTargetTemp(80.0f);
    // Установим текущую температуру в 78°C (без цикла)
    // Начальная — 20, поэтому добавим 58
    kettle.increaseTemp(58.0f); // 20 + 58 = 78
    float before = kettle.getCurrentTemp();

    PreciseHeating strategy;
    strategy.heat(kettle);

    EXPECT_NEAR(kettle.getCurrentTemp(), before + 0.3f, 1e-5f);
}

// === Тесты Kettle ===
TEST(KettleTest, TurnOnWithLowWater_SendsCriticalAlertAndDoesNotTurnOn)
{
    auto mock = std::make_shared<MockChannel>();
    auto notifier = std::make_shared<NotificationManager>();
    notifier->subscribe(mock, Priority::HIGH);

    Kettle kettle;
    kettle.setNotifier(notifier);
    kettle.setWaterLevel(0.05f); // < 0.1 → мало воды

    kettle.turnOn();

    EXPECT_TRUE(mock->hasMessage("CRITICAL: Not enough water!"));
    EXPECT_FALSE(kettle.isOn());
}

TEST(KettleTest, TurnOnReachesTarget_TurnsOffAndSendsNotifications)
{
    auto mock = std::make_shared<MockChannel>();
    auto notifier = std::make_shared<NotificationManager>();
    notifier->subscribe(mock, Priority::LOW);

    Kettle kettle;
    kettle.setNotifier(notifier);
    kettle.setTargetTemp(25.0f); // чуть выше начальных 20°C
    kettle.setHeatingStrategy(std::make_unique<PreciseHeating>());

    kettle.turnOn();

    EXPECT_FALSE(kettle.isOn());
    EXPECT_TRUE(mock->hasMessage("Target temperature reached!"));
    EXPECT_TRUE(mock->hasMessage("Kettle turned OFF"));
}

TEST(KettleTest, OverheatDetection_TurnsOffAndAlerts)
{
    auto mock = std::make_shared<MockChannel>();
    auto notifier = std::make_shared<NotificationManager>();
    notifier->subscribe(mock, Priority::HIGH);

    Kettle kettle;
    kettle.setNotifier(notifier);
    kettle.setTargetTemp(120.0f); // выше безопасного предела
    kettle.setHeatingStrategy(std::make_unique<FastHeating>());

    kettle.turnOn();

    EXPECT_FALSE(kettle.isOn());
    EXPECT_TRUE(mock->hasMessage("CRITICAL: Overheat detected!"));
}

// === Тесты NotificationManager ===
TEST(NotificationManagerTest, LowPriorityMessageNotSentToHighPriorityChannel)
{
    auto email = std::make_shared<MockChannel>();
    auto manager = std::make_shared<NotificationManager>();
    manager->subscribe(email, Priority::HIGH);

    manager->notify("Test", Priority::LOW);

    EXPECT_TRUE(email->messages.empty());
}

TEST(NotificationManagerTest, HighPriorityMessageSentToLowPriorityChannel)
{
    auto sound = std::make_shared<MockChannel>();
    auto manager = std::make_shared<NotificationManager>();
    manager->subscribe(sound, Priority::LOW);

    manager->notify("Alert!", Priority::HIGH);

    EXPECT_FALSE(sound->messages.empty());
    EXPECT_TRUE(sound->hasMessage("Alert!"));
}

TEST(NotificationManagerTest, UnsubscribeRemovesAllChannelsWithGivenName)
{
    auto mock1 = std::make_shared<MockChannel>();
    auto mock2 = std::make_shared<MockChannel>();
    auto manager = std::make_shared<NotificationManager>();
    manager->subscribe(mock1, Priority::MEDIUM);
    manager->subscribe(mock2, Priority::LOW);

    manager->unsubscribe("Mock");

    manager->notify("Test", Priority::MEDIUM);

    EXPECT_TRUE(mock1->messages.empty());
    EXPECT_TRUE(mock2->messages.empty());
}