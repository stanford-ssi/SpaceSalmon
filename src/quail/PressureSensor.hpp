#pragma once

#include "Sensor.hpp"

class PressureSensor: public Sensor{
    public:
        PressureSensor(const char* ch_name, Ad7124::InputSel ainp);

        float convertToFloat(uint32_t adc_dataword);
        static void configure();

        typedef enum {
            RANGE_1000,
            RANGE_2000
        } PressureRange;

    private:
        PressureRange range; //TODO - make different conversion function based on range

        static uint8_t cfg; // this sensor type's configuration
};

#include "main.hpp"
