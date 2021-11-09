#pragma once

#include "Sensor.hpp"
#include "ADCTask.hpp"
#include "ad7124-lib/ad7124.h"

class PressureSensor: public Sensor{
    public:
        PressureSensor(std::string ch_name, Ad7124::InputSel ainp);

        float convertToFloat(uint32_t adc_dataword);
        static void configure();

    private:
        typedef enum {
            RANGE_1000,
            RANGE_2000
        } PressureRange;
        PressureRange range;

        static uint8_t cfg;
};