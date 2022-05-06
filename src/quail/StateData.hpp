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
    void setSensorState(uint8_t ch_id, float ch_val); // sets the corresponding state value for a given sensor
    void setSolenoidState(uint8_t sol_ch, solenoid_state_t solenoid_state, bool updateSVs = true); // sets the corresponding desired state for a given solenoid
    void setBatteryCurrentState(float current); // sets the battery current state value
    void setBatteryVoltageState(float current); // sets the battery voltage state value
    void fireEmatch(uint8_t ematch_ch, bool updateEMs = true); // sets an ematch state to "fired"
    float getSensorState(uint8_t ch_id); // returns the corresponding state value for a given sensor
    uint16_t getSolenoidState(); // returns the bit map of solenoid states
    uint8_t getEmatchState(); // returns the bit map of ematch states
    float getBatteryCurrentState(); // returns the bit map of battery voltage state
    float getBatteryVoltageState(); // returns the bit map of battery current state
    void setSequence(const char* seq_name); // sets the loaded sequence name
    char* getSequence(); // returns the loaded sequence name
    void clearSequence(); // clears the loaded sequence name

    void setError(const char* error_msg); // set the error string
    char* getError(); // returns pointer to head of error string
    void clearError(); // clear the error message
    
private:
    float sensorstate [16]; // array of floats storing sensor values using their ADC channel id as index (16 possible channels)
    float batterycurrentstate; // float storing battery current measurement
    float batteryvoltagestate; // float storing battery voltage measurement
    static StaticJsonDocument<1024> stateJSON; // static JSON used to store sensor data for TX
    uint16_t solenoidstate; // 16bit indicator of solenoid status (LSB is first solenoid channel, MSB is last channel)
    uint8_t ematchstate; // 8bit indicator of ematch status (LSB is first ematch channel, MSB is last channel)
    char currSequence[MAX_TEST_FILENAME_SIZE+1];
    static Mutex SVmutex; // mutex for solenoid valve state
    static Mutex EMmutex; // mutex for ematch state
    static Mutex BatteryCurrentMutex; // mutex for battery current state
    static Mutex BatteryVoltageMutex; // mutex for battery voltage state
    static Mutex senseMutex; // mutex for sensor state
    static Mutex seqMutex; // mutex for sequencing (state and name)
    static Mutex errMutex; // mutex for error
    char error_buf[ERR_BUF_SIZE]; // string for storing errors
};