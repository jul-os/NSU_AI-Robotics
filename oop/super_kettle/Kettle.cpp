#include "Kettle.h"
#include <thread>
#include <chrono>
#include <algorithm>
#include <iostream>

namespace
{
    constexpr float MIN_WATER_LEVEL = 0.1f;
    constexpr float DEFAULT_START_TEMP = 20.0f;
    constexpr float BOILING_TEMP = 100.0f;
    constexpr float OVERHEAT_THRESHOLD = 110.0f;
    constexpr float TARGET_TOLERANCE = 0.5f;
    constexpr int HEAT_LOOP_DELAY_MS = 200;
}

Kettle::Kettle()
    : currentTemp_(DEFAULT_START_TEMP), targetTemp_(BOILING_TEMP), waterLevel_(1.0f), isOn_(false), heatingStrategy_(std::make_unique<FastHeating>())
{
}
void Kettle::setHeatingStrategy(std::unique_ptr<HeatingStrategy> strategy)
{
    heatingStrategy_ = std::move(strategy);
}

void Kettle::setNotifier(std::shared_ptr<NotificationManager> notifier)
{
    notifier_ = notifier;
}
void Kettle::turnOn()
{
    if (waterLevel_ < MIN_WATER_LEVEL)
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
        std::this_thread::sleep_for(std::chrono::milliseconds(HEAT_LOOP_DELAY_MS));
    }

    // проверяем после выхода из цикла что не перескочили цель
    if (isOn_)
    {
        checkTargetReached();
        checkSafety();
    }
}

void Kettle::turnOff()
{
    if (isOn_)
    {
        isOn_ = false;
        notifier_->notify("Kettle turned OFF", Priority::LOW);
    }
}

void Kettle::increaseTemp(float delta)
{
    currentTemp_ += delta;
}

void Kettle::decreaseWater(float amount)
{
    waterLevel_ = std::max(0.0f, waterLevel_ - amount);
}

void Kettle::checkTargetReached()
{
    if (currentTemp_ >= targetTemp_ - TARGET_TOLERANCE)
    {
        turnOff();
        notifier_->notify("Target temperature reached!", Priority::MEDIUM);
    }
}

void Kettle::checkSafety()
{
    if (waterLevel_ < MIN_WATER_LEVEL)
    {
        notifier_->notify("CRITICAL: Low water level!", Priority::HIGH);
        turnOff();
    }
    if (currentTemp_ > OVERHEAT_THRESHOLD)
    {
        notifier_->notify("CRITICAL: Overheat detected!", Priority::HIGH);
        turnOff();
    }
}

void Kettle::setTargetTemp(float temp)
{
    targetTemp_ = temp;
}

void Kettle::setWaterLevel(float level)
{
    waterLevel_ = level;
}

void Kettle::setCurrentTemp(float temp)
{
    currentTemp_ = temp;
}

float Kettle::getCurrentTemp() const
{
    return currentTemp_;
}

float Kettle::getWaterLevel() const
{
    return waterLevel_;
}

float Kettle::getTargetTemp() const
{
    return targetTemp_;
}

bool Kettle::isOn() const
{
    return isOn_;
}