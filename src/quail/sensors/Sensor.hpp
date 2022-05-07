#pragma once

#include "ad7124-lib/ad7124.h"
#include "SlateKey.hpp"
#include "../slate/SlateAbstractions.hpp"

class Sensor {
    public:
        /**
         * @brief General constructor for sensors, autoincrements number of sensors
         */
        Sensor(const char* ch_name, Ad7124::InputSel ainp, Ad7124::InputSel ainm, SensorSlate& slate);         

        /**
         * @brief over-written by inheritors, returns SI unit value of reading from ADC bin count
         */
        virtual void convertToFloat(uint32_t adc_dataword); 

        /**
         * @brief Configures and initializes this sensor.
         */
        virtual void configure(); 

        const char* ch_name; // data channel name

        static uint8_t numSensors(){ return num_sensors; };

    protected:   
        void updateSlate();

        const uint8_t ch_id; // ADC channel ID
        const Ad7124::InputSel ainp; // ADC positive input
        const Ad7124::InputSel ainm; // ADC negative input  

        SensorSlate slate; // slate channel

        static uint8_t addConfig() { return num_cfgs++; }

    private:
        static uint8_t num_sensors; // running count of number of sensors initialized
        static uint8_t num_cfgs; // number of sensor configurations set up on ADC
};