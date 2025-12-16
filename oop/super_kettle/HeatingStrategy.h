// HeatingStrategy.h
#ifndef HEATING_STRATEGY_H
#define HEATING_STRATEGY_H

#include <memory>

class Kettle; // forward declaration — достаточно для ссылки

class HeatingStrategy
{
public:
    virtual ~HeatingStrategy() = default;
    virtual void heat(Kettle &kettle) const = 0;
};

class FastHeating : public HeatingStrategy
{
public:
    void heat(Kettle &kettle) const override;
};

class EcoHeating : public HeatingStrategy
{
public:
    void heat(Kettle &kettle) const override;
};

class PreciseHeating : public HeatingStrategy
{
public:
    void heat(Kettle &kettle) const override;
};

#endif // HEATING_STRATEGY_H