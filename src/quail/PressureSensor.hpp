#pragma once

#include "Sensor.hpp"
#include "ADCTask.hpp"
#include "ad7124-lib/ad7124.h"

class PressureSensor: public Sensor{
public:
    typedef enum {
        RANGE_1000,
        RANGE_2000
    } PressureRange;
    PressureSensor(uint8_t channel, const char* ch_name, Ad7124::InputSel ainp, PressureRange range):
        Sensor(channel,ch_name,ainp,Ad7124::AVSSInput)
    {
        this->range = range;
        this->cfg = PRESSURE_CFG;
    };
    float convertToFloat(uint32_t adc_dataword)
    {
        double voltage = Ad7124Chip::toVoltage(adc_dataword, 1, 2.5, false);
        voltage = voltage * 1.5;
        // TODO: make this calculation variable based on range, set by this->range
        float pressure = (voltage - 0.5) / 4.0 * 1000;
        return pressure;
    };
private:
    PressureRange range;
};