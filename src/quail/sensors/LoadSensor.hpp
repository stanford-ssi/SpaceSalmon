#pragma once

#include "Sensor.hpp"

class LoadSensor: public Sensor{
    public:
        LoadSensor(const char* ch_name, Ad7124::InputSel ainp, SlateKey<float> &slate_channel);

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