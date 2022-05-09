#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include "event_groups.h"
#include <hal_rtos.h>

#include "Task.hpp"
#include "../config.h"


class ADCTask : public Task<2000>
{
public:
    ADCTask(uint8_t priority);

    struct {
        uint32_t dataword;
        uint8_t channel;
    } typedef adcdata_t;

    /**
     * @brief set flag to indicate that all sensor channels have been configured. This is meant to be called from another task (SensorTask).
     */
    void sensorsConfigured();

private:
    enum events{
        NewData = 0b10,
        SensorsReady = 0b01
    };

    static void adcISR();

    void activity();
    
    /**
     * @brief Restore ADC to a status where it can be initialized
     */
    void resetADC();
    
    /**
     * @brief Initializes the ADC, after execution ready to process data
     */ 
    void initADC();
    
    StaticEventGroup_t evbuf;
    EventGroupHandle_t evgroup;

    uint8_t timeout_count = 0;
    uint8_t err_count = 0;
    uint8_t data_count = 0;

};

#include "ArduinoJson.h"
