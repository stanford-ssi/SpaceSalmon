#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "MsgBuffer.hpp"
#include <semphr.h>
#include <hal_rtos.h>
#include <string>
#include "ArduinoJson.h"

class StateTask
{
public:
    StaticJsonDocument<1024> getState(); // returns data packet of current state for telem output
    void setSensorState(uint8_t ch_id, float ch_val); // sets the corresponding state value for a given sensor
    void setSolenoidState(uint8_t sol_ch, bool solenoid_open); // sets the corresponding desired state for a given solenoid
    void setEmatchState(uint8_t ematch_ch, bool fire_ematch); // sets the corresponding desired state for a given ematch
    float getSensorState(uint8_t ch_id); // returns the corresponding state value for a given sensor
    bool getSolenoidState(uint8_t sol_ch); // returns the corresponding state for a given solenoid
    bool getEmatchState(uint8_t ematch_ch); // returns the corresponding desired state for a given ematch
    
private:
    float sensorstate [16]; // array of floats storing sensor values using their ADC channel id as index (16 possible channels)
    uint8_t solenoidstate; // 8bit indicator of solenoid status (LSB is first solenoid channel, MSB is last channel)
    uint8_t ematchstate; // 8bit indicator of ematch status (LSB is first ematch channel, MSB is last channel)
    StaticJsonDocument<1024> statepacket;
};

#include "main.hpp"
