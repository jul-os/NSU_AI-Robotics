// Kettle.h — ПОЛНАЯ РЕАЛИЗАЦИЯ В ЗАГОЛОВКЕ
#ifndef KETTLE_H
#define KETTLE_H

#include "HeatingStrategy.h"
#include "NotificationManager.h"
#include <memory>
#include <thread>
#include <chrono>
#include <algorithm>
#include <iostream>

class Kettle
{
public:
    Kettle()
        : currentTemp_(20.0f), targetTemp_(100.0f), waterLevel_(1.0f), isOn_(false), heatingStrategy_(std::make_unique<FastHeating>())
    {
    }

    void setHeatingStrategy(std::unique_ptr<HeatingStrategy> strategy)
    {
        heatingStrategy_ = std::move(strategy);
    }

    void setNotifier(std::shared_ptr<NotificationManager> notifier)
    {
        notifier_ = notifier;
    }

    void setTargetTemp(float temp) { targetTemp_ = temp; }
    float getCurrentTemp() const { return currentTemp_; }
    float getWaterLevel() const { return waterLevel_; }
    float getTargetTemp() const { return targetTemp_; }
    bool isOn() const { return isOn_; }
    void setWaterLevel(float level) { waterLevel_ = level; }
    void turnOn()
    {
        if (waterLevel_ < 0.1f)
        {
            notifier_->notify("CRITICAL: Not enough water!", Priority::HIGH);
            return;
        }
        isOn_ = true;
        notifier_->notify("Kettle turned ON", Priority::LOW);

        while (isOn_ && currentTemp_ < targetTemp_)
        {
            heatingStrategy_->heat(*this);
            checkSafety();
            // checkTargetReached(); // уберём отсюда — будем проверять в любом случае после цикла
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        // ВАЖНО: проверяем после выхода из цикла — вдруг перескочили цель?
        if (isOn_)
        {
            checkTargetReached(); // теперь вызовётся даже если currentTemp >= targetTemp
            // Если всё ещё включён и не было критической ошибки — возможно, уже достигнуто
            checkSafety();
        }
    }

    void turnOff()
    {
        if (isOn_)
        {
            isOn_ = false;
            notifier_->notify("Kettle turned OFF", Priority::LOW);
        }
    }

    void increaseTemp(float delta)
    {
        currentTemp_ += delta;
    }

    void decreaseWater(float amount = 0.01f)
    {
        waterLevel_ = std::max(0.0f, waterLevel_ - amount);
    }

private:
    void checkTargetReached()
    {
        if (currentTemp_ >= targetTemp_ - 0.5f)
        {
            turnOff();
            notifier_->notify("Target temperature reached!", Priority::MEDIUM);
        }
    }

    void checkSafety()
    {
        if (waterLevel_ < 0.1f)
        {
            notifier_->notify("CRITICAL: Low water level!", Priority::HIGH);
            turnOff();
        }
        if (currentTemp_ > 110.0f)
        {
            notifier_->notify("CRITICAL: Overheat detected!", Priority::HIGH);
            turnOff();
        }
    }

    // Для тестов
    friend class KettleTest_StrategyPrecise_Test;
    friend class KettleTest_TurnOnReachesTarget_Test;
    // ... или лучше — friend всем тестам через макрос, но для простоты:

    float currentTemp_;
    float targetTemp_;
    float waterLevel_;
    bool isOn_;
    std::unique_ptr<HeatingStrategy> heatingStrategy_;
    std::shared_ptr<NotificationManager> notifier_;
};

// --- Реализации стратегий (после определения Kettle) ---
void FastHeating::heat(Kettle &kettle) const
{
    kettle.increaseTemp(3.0f);
    kettle.decreaseWater(0.02f);
}

void EcoHeating::heat(Kettle &kettle) const
{
    kettle.increaseTemp(1.0f);
    kettle.decreaseWater(0.005f);
}

void PreciseHeating::heat(Kettle &kettle) const
{
    float diff = kettle.getTargetTemp() - kettle.getCurrentTemp();
    float step = (diff > 5.0f) ? 2.0f : 0.3f;
    kettle.increaseTemp(step);
    kettle.decreaseWater(0.01f);
}

#endif // KETTLE_H