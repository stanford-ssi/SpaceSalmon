#pragma once

#include "ad7124-lib/ad7124.h"
#include "Task.hpp"
#include "MsgBuffer.hpp"

#define UNCONFIGURED UINT8_MAX

class Sensor:Task<500> {
    public:
        static uint8_t sensor_priority; // priority of all sensor classes

        /**
         * @brief General constructor for sensors, autoincrements number of sensors
         */
        Sensor(const char* ch_name, Ad7124::InputSel ainp, Ad7124::InputSel ainm) 
            : Task(sensor_priority, "Sensor"), ch_id(num_sensors++), ch_name(ch_name), ainp(ainp), ainm(ainm){}; 

        /**
         * @brief over-written by inheritors, returns SI unit value of reading from ADC bin count
         */
        virtual float convertToFloat(uint32_t adc_dataword); 

        /**
         * @brief Tells ADC how to generally handle sensors of this type. 
         * Parent just increments number of configs. Children actually set them up
         */
        void configure(); 

        /**
         *  @return The actual value of this sensor.
         */
        float get_value(){ return sensor_value; };

        /**
         * @brief Add data to the adcbuf for conversion.
         */
        void addADCdata(uint32_t adc_data){ adcbuf.send(adc_data); };

        void activity(){
            while(true){
                uint32_t adc_data;
                adcbuf.receive(adc_data, true); // wait for data to arrive
                sensor_value = this->convertToFloat(adc_data); 
                Serial.println(sensor_value);
            }
        };

    protected:
        static uint8_t add_config() {return ++Sensor::num_cfgs;}

        float sensor_value; // actual value of sensor reading
        MsgBuffer<uint32_t, 100> adcbuf; // buffer of raw adc data, filled via addADCdata()

        static uint8_t num_sensors; // running count of number of sensors
        static uint8_t num_cfgs; // running count of number of configurations

        const uint8_t ch_id; // ADC channel ID
        const char* ch_name; // data channel name
        const Ad7124::InputSel ainp; // ADC positive input
        const Ad7124::InputSel ainm; // ADC negative input     
};
uint8_t Sensor::num_cfgs = 0;
uint8_t Sensor::num_sensors = 0;

#include "main.hpp"
