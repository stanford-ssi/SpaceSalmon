#pragma once

#include "Sensor.hpp"

typedef enum {
    RANGE_1000,
    RANGE_2000
} PressureRange;

class PressureSensor: public Sensor{
    public:

        PressureSensor(const char* ch_name, Ad7124::InputSel ainp, PressureRange range);

        float convertToFloat(uint32_t adc_dataword);
        void configure();

    private:
        PressureRange range; //TODO - make different conversion function based on range

        static uint8_t cfg; // this sensor type's configuration
};

#include "main.hpp"
