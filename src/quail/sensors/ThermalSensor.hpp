#pragma once

#include "Sensor.hpp"
#include "../config.h"

class ThermalSensor : public Sensor {
    public:

        ThermalSensor(Ad7124::InputSel ainp, SlateField<float>& slate);

        void convertToFloat(uint32_t adc_dataword);
        void configure();
        static void unconfigure(){cfg = UNCONFIGURED;};

    private:
        // const TemperatureRange range; //TODO - make different conversion function based on range

        static uint8_t cfg; // this sensor type's configuration
};