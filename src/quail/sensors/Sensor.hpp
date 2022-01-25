#pragma once

#include "ad7124-lib/ad7124.h"

#define UNCONFIGURED UINT8_MAX

class Sensor {
    public:
        /**
         * @brief General constructor for sensors, autoincrements number of sensors
         */
        Sensor(const char* ch_name, Ad7124::InputSel ainp, Ad7124::InputSel ainm) 
            : ch_name(ch_name), ch_id(num_sensors), ainp(ainp), ainm(ainm) {num_sensors++;}; 

        /**
         * @brief over-written by inheritors, returns SI unit value of reading from ADC bin count
         */
        virtual float convertToFloat(uint32_t adc_dataword); 

        /**
         * @brief Configures and initializes this sensor.
         */
        virtual void configure(); 

        /**
         *  @return The actual value of this sensor.
         */
        float getValue(){ return sensor_value; };

        const char* ch_name; // data channel name

        static uint8_t numSensors(){ return num_sensors; };

    protected:
        float sensor_value; // actual value of sensor reading

        const uint8_t ch_id; // ADC channel ID
        const Ad7124::InputSel ainp; // ADC positive input
        const Ad7124::InputSel ainm; // ADC negative input    

        static uint8_t num_configs; // number of sensor configurations set up on ADC
        static uint8_t num_sensors; // running count of number of sensors initialized

        static uint8_t add_config(){ return num_configs++; }; // increment number of configs
};