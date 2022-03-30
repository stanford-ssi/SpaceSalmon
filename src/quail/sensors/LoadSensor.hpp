#pragma once

#include "Sensor.hpp"
#include "../config.h"

class LoadSensor : public Sensor{
    public:
        LoadSensor(Ad7124::InputSel ainp, SensorSlate &slate);

        void convertToFloat(uint32_t adc_dataword);
        
        void configure();
        static void unconfigure(){cfg = UNCONFIGURED;};

    private:
        // typedef enum {
        //     RANGE_1000,
        //     RANGE_2000
        // } LoadRange;
        // LoadRange range;

        static uint8_t cfg;
};