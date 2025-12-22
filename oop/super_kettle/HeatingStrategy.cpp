#include "HeatingStrategy.h"
#include "Kettle.h"

namespace
{
    constexpr float ECO_TEMP_STEP = 0.1f;
    constexpr float INCREASE_TEMP = 3.0f;
    constexpr float FAST_DECREASE_WATER = 0.02f;
    constexpr float ECO_DECREASE_WATER = 0.005f;
    constexpr float TARGET_TOLERANCE = 0.5f;
    constexpr float BIG_STEP = 2.0f;
    constexpr float SMOL_STEP = 0.3f;
    constexpr float PRECISE_DECREASE_WATER = 0.01f;
}

void FastHeating::heat(Kettle &kettle) const
{
    kettle.increaseTemp(INCREASE_TEMP);
    kettle.decreaseWater(FAST_DECREASE_WATER);
}

void EcoHeating::heat(Kettle &kettle) const
{
    kettle.increaseTemp(ECO_TEMP_STEP);
    kettle.decreaseWater(ECO_DECREASE_WATER);
}

void PreciseHeating::heat(Kettle &kettle) const
{
    const float diff = kettle.getTargetTemp() - kettle.getCurrentTemp();
    const float step = (diff > TARGET_TOLERANCE) ? BIG_STEP : SMOL_STEP;
    kettle.increaseTemp(step);
    kettle.decreaseWater(PRECISE_DECREASE_WATER);
}