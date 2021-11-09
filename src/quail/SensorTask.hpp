#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "MsgBuffer.hpp"
#include <semphr.h>
#include <hal_rtos.h>

#include "Task.hpp"
#include "Sensor.hpp"

#define NUM_SENSORS 1 // define the total number of sensors here!

 struct {
    uint8_t channel = 0;
    uint32_t dataword = 0;
} typedef adcdata_t;

class SensorTask : public Task<1000> {
    public:
        SensorTask(uint8_t priority);
        static void initialize(); // set up all sensor adc channels based on static sensors array
        void addADCData(adcdata_t adcdata); // adds an adc packet to the msgbuf to be processed and sent to state
        
    private:
        void activity();
        MsgBuffer<adcdata_t, 1000>  adcbuf; //buffer for adc data
        static Sensor* sensors[NUM_SENSORS];
};
#include "main.hpp"
