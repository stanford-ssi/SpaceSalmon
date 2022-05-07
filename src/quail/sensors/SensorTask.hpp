#pragma once

#include "Task.hpp"
#include "MsgBuffer.hpp"
#include "event_groups.h"
#include "ADCTask.hpp"
#include "Sensor.hpp"

class SensorTask: public Task<1000>{
public:
    SensorTask(uint8_t priority);   
    
    /**
     * @brief Get sensors ready for configuration, called by ADCTask
     */
    static void initSensors();
    
    /** 
     * @brief Configure all sensors in sys.sensors.
     */
    void configureSensors();

    /**
     * @brief Indicate that ADC has looped through all sensor values and data should be pulled from buffer.
     */
    static void dataReady();

    /**
     * @brief Add data to the adcbuf for conversion.
     */
    void addADCdata(ADCTask::adcdata_t adc_data){ adcbuf.send(adc_data); };

    /**
     * @brief Read data from all sensors
     */
    bool readData();

    void activity();

private:
    MsgBuffer<ADCTask::adcdata_t, 100> adcbuf; // buffer of raw adc data, filled via addADCdata()

    static StaticEventGroup_t evbuf;
    static EventGroupHandle_t evgroup; // event group to await ADC ready and check for ADC reset
};