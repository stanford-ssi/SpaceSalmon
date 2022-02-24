#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "MsgBuffer.hpp"
#include "Mutex.hpp"
#include <semphr.h>
#include <hal_rtos.h>
#include "ArduinoJson.h"

#include "SequenceTask.hpp" // included for max size of sequence filename
#include "actuators/ValveTask.hpp" // included for solenoid_state_t def

#define ERR_BUF_SIZE 40

class StateData
{
public:
    static StaticJsonDocument<1024>* getState(); // returns json data packet of current state for telem output
    void setSolenoidState(uint8_t sol_ch, solenoid_state_t solenoid_state, bool updateSVs = true); // sets the corresponding desired state for a given solenoid
    void setBatteryCurrentState(float current); // sets the battery current state value
    void setBatteryVoltageState(float current); // sets the battery voltage state value
    void fireEmatch(uint8_t ematch_ch, bool updateEMs = true); // sets an ematch state to "fired"
    uint8_t getSolenoidState(); // returns the bit map of ematch states
    uint8_t getEmatchState(); // returns the bit map of ematch states

    void setError(const char* error_msg); // set the error string
    char* getError(); // returns pointer to head of error string
    void clearError(); // clear the error message
    
private:
    static StaticJsonDocument<1024> stateJSON; // static JSON used to store sensor data for TX
    uint8_t solenoidstate; // 8bit indicator of solenoid status (LSB is first solenoid channel, MSB is last channel)
    uint8_t ematchstate; // 8bit indicator of ematch status (LSB is first ematch channel, MSB is last channel)
    static Mutex SVmutex; // mutex for solenoid valve state
    static Mutex EMmutex; // mutex for ematch state
    static Mutex errMutex; // mutex for error
    char error_buf[ERR_BUF_SIZE]; // string for storing errors
};