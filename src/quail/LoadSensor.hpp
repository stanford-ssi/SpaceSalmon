#pragma once

#include "Sensor.hpp"
#include "ADCTask.hpp"
#include "ad7124-lib/ad7124.h"

class LoadSensor: public Sensor{
    public:
        LoadSensor(std::string ch_name, Ad7124::InputSel ainp, Ad7124::InputSel ainm);

        float convertToFloat(uint32_t adc_dataword);
        static void configure();

    private:
        typedef enum {
            RANGE_1000,
            RANGE_2000
        } LoadRange;
        LoadRange range;

        static uint8_t cfg;
        uint8_t channel;
};