#pragma once

class Sensor;

#include "ad7124-lib/ad7124.h"
#include <cstdint>
#include <string>
#include "Task.hpp"


class Sensor {
    public:
        /**
         * @brief General constructor for sensors, autoincrements number of sensors
         */
        Sensor(std::string ch_name, Ad7124::InputSel ainp, Ad7124::InputSel ainm) 
            : ch_id(num_sensors++), ch_name(ch_name), ainp(ainp), ainm(ainm){}; 

        /**
         * @brief over-written by inheritors, returns SI unit value of reading from ADC bin count
         */
        virtual float convertToFloat(uint32_t adc_dataword); 

        /**
         * @brief Tells ADC how to generally handle sensors of this type. 
         * Parent just increments number of configs. Children actually set them up
         */
        void configure(); 
        static int add_config() {return ++Sensor::num_cfgs;}

        void activity();

    protected:
        static uint8_t num_sensors; // running count of number of sensors
        static uint8_t num_cfgs; // running count of number of configurations

        const uint8_t ch_id; // ADC channel ID
        const std::string ch_name; // data channel name
        const Ad7124::InputSel ainp; // ADC positive input
        const Ad7124::InputSel ainm; // ADC negative input     

    private:
        friend class SensorTask;   
};
#include "main.hpp"
