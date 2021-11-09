#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "MsgBuffer.hpp"
#include <semphr.h>
#include <hal_rtos.h>
#include <string>
#include "ArduinoJson.h"

#include "Task.hpp"

struct
{  
    float value = 0.0;
    char* ch_id;
} typedef sensordata_t;

class StateTask : public Task<1000>
{
public:
    StateTask(uint8_t priority);
    StaticJsonDocument<1024> getState(); // returns data packet of current state for telem output
    void updateSensorState(sensordata_t sensedata); // adds sensor data packet to sensorbuf
    
private:
    void activity();  // every 1ms or so, check for updates and send state to logger
    StaticJsonDocument<1024> state;
    MsgBuffer<sensordata_t,1000> sensorbuf;
};

#include "main.hpp"
