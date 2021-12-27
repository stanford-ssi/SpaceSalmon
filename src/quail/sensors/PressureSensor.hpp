#pragma once

#include "Sensor.hpp"

#define PSI_TO_PA 6894.7572931783

typedef enum {
    RANGE_1000 = 1000,
    RANGE_2000 = 2000
} PressureRange;

class PressureSensor: public Sensor{
    public:

        PressureSensor(const char* ch_name, Ad7124::InputSel ainp, PressureRange range);

        float convertToFloat(uint32_t adc_dataword);
        void configure();

    private:
        const PressureRange range; //TODO - make different conversion function based on range

        static uint8_t cfg; // this sensor type's configuration
};