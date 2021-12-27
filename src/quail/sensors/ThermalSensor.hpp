#pragma once

#include "Sensor.hpp"

class ThermalSensor: public Sensor{
    public:

        ThermalSensor(const char* ch_name, Ad7124::InputSel ainp);

        float convertToFloat(uint32_t adc_dataword);
        void configure();

    private:
        // const TemperatureRange range; //TODO - make different conversion function based on range

        static uint8_t cfg; // this sensor type's configuration
};