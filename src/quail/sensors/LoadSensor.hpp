#pragma once

#include "Sensor.hpp"

class LoadSensor: public Sensor{
    public:
        LoadSensor(char* ch_name, Ad7124::InputSel ainp);

        float convertToFloat(uint32_t adc_dataword);
        void configure();

    private:
        // typedef enum {
        //     RANGE_1000,
        //     RANGE_2000
        // } LoadRange;
        // LoadRange range;

        static uint8_t cfg;
};