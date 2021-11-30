#pragma once

#include "SPI.h"
#include "ad7124-lib/ad7124.h"
#include "Task.hpp"
#include "MsgBuffer.hpp"
#include "event_groups.h"
#include "Mutex.hpp"

#define UNCONFIGURED UINT8_MAX
#define ADC_STARTED 0b01
#ifndef SENSOR_PRIORITY
    #define SENSOR_PRIORITY 2
#endif

class Sensor:Task<1000> {
    public:
        /**
         * @brief General constructor for sensors, autoincrements number of sensors
         */
        Sensor(const char* ch_name, Ad7124::InputSel ainp, Ad7124::InputSel ainm) 
            : Task(sensor_priority, "Sensor"), ch_name(ch_name), ch_id(num_sensors), ainp(ainp), ainm(ainm){num_sensors++;}; 

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
        float get_value(){ return sensor_value; };

        /**
         * @brief Add data to the adcbuf for conversion.
         */
        void addADCdata(uint32_t adc_data){ adcbuf.send(adc_data); };

        static void ADCbegin();

        void activity();

        static uint8_t num_sensors; // running count of number of sensors initialized
        const char* ch_name; // data channel name

    protected:
        static void _configure(Sensor* this_sense); //wrapper function for children's implementation of configure()
        static uint8_t add_config(){return ++num_cfgs;};
        static Mutex config_mx; 

        float sensor_value; // actual value of sensor reading
        MsgBuffer<uint32_t, 100> adcbuf; // buffer of raw adc data, filled via addADCdata()

        static uint8_t num_cfgs; // running count of number of ADC configurations 
        static uint8_t num_sens_cfgd; // running count of the number of sensors initialized and configured
        static uint8_t sensor_priority; // priority of all sensor classes

        const uint8_t ch_id; // ADC channel ID
        const Ad7124::InputSel ainp; // ADC positive input
        const Ad7124::InputSel ainm; // ADC negative input    

        static StaticEventGroup_t evbuf;
        static EventGroupHandle_t evgroup;
};
