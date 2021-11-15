#include "StateTask.hpp"

StaticJsonDocument<1024> StateTask::getState()
{
    // Update statepacket from sensor, solenoid, and ematch statuses
    for(uint8_t i = 0; i < Sensor::num_sensors; i++)
        statepacket[(sys.sensors[i])->ch_name] = getSensorState(i);
    statepacket["SV"] = solenoidstate; // most efficient way to send is just as the raw uint, can decode on groundside
    statepacket["EM"] = ematchstate; // most efficient way to send is just as the raw uint, can decode on groundside
    return statepacket;
};

void StateTask::setSensorState(uint8_t ch_id, float ch_val)
{
    sensorstate[ch_id] = ch_val;
};

void StateTask::setSolenoidState(uint8_t sol_ch, bool solenoid_open)
{
    // left shift the bit indicator for status to the correct location and set that bit
    solenoidstate |= (solenoid_open << sol_ch);
};

void StateTask::setEmatchState(uint8_t ematch_ch, bool fire_ematch)
{
    // left shift the bit indicator for status to the correct location and set that bit
    ematchstate |= (fire_ematch << ematch_ch);
};

float StateTask::getSensorState(uint8_t ch_id)
{
    return sensorstate[ch_id];
};

bool StateTask::getSolenoidState(uint8_t sol_ch)
{
    return (solenoidstate & (0b1 << sol_ch) ) >> sol_ch;
};

bool StateTask::getEmatchState(uint8_t ematch_ch)
{
    return (ematchstate & (0b1 << ematch_ch) ) >> ematch_ch;
};