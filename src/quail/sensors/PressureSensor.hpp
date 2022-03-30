#pragma once

#include "Sensor.hpp"
#include "../config.h"

typedef enum {
    RANGE_1000 = 1000,
    RANGE_2000 = 2000
} PressureRange;

class PressureSensor : public Sensor{
    public:
        PressureSensor(Ad7124::InputSel ainp, PressureRange range, SensorSlate &slate);

        void convertToFloat(uint32_t adc_dataword);
        void configure();

        static void unconfigure(){cfg = UNCONFIGURED;};

    private:
        const PressureRange range; //TODO - make different conversion function based on range

        static uint8_t cfg; // this sensor type's configuration
};
