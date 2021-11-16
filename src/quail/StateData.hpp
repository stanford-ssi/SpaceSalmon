#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "MsgBuffer.hpp"
#include "Mutex.hpp"
#include <semphr.h>
#include <hal_rtos.h>
#include <string>
#include "ArduinoJson.h"

typedef enum{
    OPEN = true,
    CLOSED = false,
} SVState_t; // state indicators for SVs to keep the nomenclature consistent

typedef enum{
    FIRED = true,
    UNFIRED = false,
} EMState_t; // state indicators for EMs to keep the nomenclature consistent

class StateData
{
public:
    StaticJsonDocument<1024> getState(); // returns json data packet of current state for telem output
    void setSensorState(uint8_t ch_id, float ch_val); // sets the corresponding state value for a given sensor
    void setSolenoidState(uint8_t sol_ch, SVState_t solenoid_state); // sets the corresponding desired state for a given solenoid
    void setEmatchState(uint8_t ematch_ch, EMState_t ematch_state); // sets the corresponding desired state for a given ematch
    float getSensorState(uint8_t ch_id); // returns the corresponding state value for a given sensor
    bool getSolenoidState(); // returns the bit map showing state of 
    bool getEmatchState(); // returns the corresponding desired state for a given ematch
    
private:
    float sensorstate [16]; // array of floats storing sensor values using their ADC channel id as index (16 possible channels)
    uint8_t solenoidstate; // 8bit indicator of solenoid status (LSB is first solenoid channel, MSB is last channel)
    uint8_t ematchstate; // 8bit indicator of ematch status (LSB is first ematch channel, MSB is last channel)
    Mutex SVmutex; // mutex for solenoid valve state
    Mutex EMmutex; // mutex for ematch state
    Mutex senseMutex; // mutex for sensor state
    StaticJsonDocument<1024> statepacket;
};