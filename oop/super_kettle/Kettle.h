#ifndef KETTLE_H
#define KETTLE_H

#include "HeatingStrategy.h"
#include "NotificationManager.h"
#include <memory>

class Kettle
{
public:
    Kettle();
    void setHeatingStrategy(std::unique_ptr<HeatingStrategy> strategy);
    void setNotifier(std::shared_ptr<NotificationManager> notifier);
    void setTargetTemp(float temp);
    void setWaterLevel(float level);
    void setCurrentTemp(float temp);
    float getCurrentTemp() const;
    float getWaterLevel() const;
    float getTargetTemp() const;
    bool isOn() const;
    void turnOn();
    void turnOff();
    void increaseTemp(float delta);
    void decreaseWater(float amount = 0.01f);

private:
    void checkTargetReached();
    void checkSafety();

    friend class KettleTest_StrategyPrecise_Test;
    friend class KettleTest_TurnOnReachesTarget_Test;

    float currentTemp_;
    float targetTemp_;
    float waterLevel_;
    bool isOn_;
    std::unique_ptr<HeatingStrategy> heatingStrategy_;
    std::shared_ptr<NotificationManager> notifier_;
};

#endif // KETTLE_H