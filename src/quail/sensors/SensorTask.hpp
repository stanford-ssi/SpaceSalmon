#pragma once

#include "Task.hpp"
#include "MsgBuffer.hpp"
#include "event_groups.h"

#define ADC_STARTED 0b001
#define ADC_RESET   0b010
#define DATA_READY  0b100

struct {
    uint32_t dataword;
    uint8_t channel;
} typedef adcdata_t;

class SensorTask: public Task<1000>{
public:
    SensorTask(uint8_t priority):Task(priority, "SensorTask"){};
    /**
     * @brief Indicate that ADC begin() function has been called since last restart.
     */
    static void ADCbegin();

    /**
     * @brief Indicate that ADC reset() function has been called, and ADC_BEGIN should be awaited.
     */
    static void ADCreset();

    /**
     * @brief Indicate that ADC has looped through all sensor values and data should be pulled from buffer.
     */
    static void dataReady();

    /**
     * @brief Add data to the adcbuf for conversion.
     */
    void addADCdata(adcdata_t adc_data){ adcbuf.send(adc_data); };

    /** 
     * @brief Configure all sensors in sys.sensors.
     */
    void configure();

    void activity();

private:
    MsgBuffer<adcdata_t, 100> adcbuf; // buffer of raw adc data, filled via addADCdata()

    static StaticEventGroup_t evbuf;
    static EventGroupHandle_t evgroup; // event group to await ADC ready and check for ADC reset
};