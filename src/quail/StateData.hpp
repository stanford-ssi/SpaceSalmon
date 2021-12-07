#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "MsgBuffer.hpp"
#include "Mutex.hpp"
#include <semphr.h>
#include <hal_rtos.h>
#include "ArduinoJson.h"

#include "ValveTask.hpp"

#define ERR_BUF_SIZE 40

class StateData
{
public:
    static StaticJsonDocument<1024>* getState(); // returns json data packet of current state for telem output
    void setSensorState(uint8_t ch_id, float ch_val); // sets the corresponding state value for a given sensor
    void setSolenoidState(uint8_t sol_ch, solenoid_state_t solenoid_state, bool updateSVs = true); // sets the corresponding desired state for a given solenoid
    void fireEmatch(uint8_t ematch_ch, bool updateEMs = true); // sets an ematch state to "fired"
    float getSensorState(uint8_t ch_id); // returns the corresponding state value for a given sensor
    uint8_t getSolenoidState(); // returns the bit map of ematch states
    uint8_t getEmatchState(); // returns the bit map of ematch states
    void setError(const char* error_msg); // set the error string
    char* getError(); // returns pointer to head of error string
    void clearError(); // clear the error message
    
private:
    float sensorstate [16]; // array of floats storing sensor values using their ADC channel id as index (16 possible channels)
    static StaticJsonDocument<1024> stateJSON; // static JSON used to store sensor data for TX
    uint8_t solenoidstate; // 8bit indicator of solenoid status (LSB is first solenoid channel, MSB is last channel)
    uint8_t ematchstate; // 8bit indicator of ematch status (LSB is first ematch channel, MSB is last channel)
    static Mutex SVmutex; // mutex for solenoid valve state
    static Mutex EMmutex; // mutex for ematch state
    static Mutex senseMutex; // mutex for sensor state
    static Mutex errMutex; // mutex for error
    char error_buf[ERR_BUF_SIZE]; // string for storing errors
};