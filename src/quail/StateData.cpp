#include "StateData.hpp"
#include "main.hpp"

StaticJsonDocument<1024> StateData::getState()
{
    // Update statepacket from sensor, solenoid, and ematch statuses
    for(uint8_t i = 0; i < Sensor::num_sensors; i++)
        statepacket[(sys.sensors[i])->ch_name] = getSensorState(i);
    statepacket["SV"] = solenoidstate; // most efficient way to send is just as the raw uint, can decode on groundside
    statepacket["EM"] = ematchstate; // most efficient way to send is just as the raw uint, can decode on groundside
    return statepacket;
};

void StateData::setSensorState(uint8_t ch_id, float ch_val)
{
    senseMutex.take(NEVER);
    sensorstate[ch_id] = ch_val;
    senseMutex.give();
};

void StateData::setSolenoidState(uint8_t sol_ch, bool solenoid_open)
{
    SVmutex.take(NEVER);
    // clear then set the bit corresponding to the desired sol_ch (zero indexed)
    solenoidstate = (solenoidstate & ~(1U << sol_ch)) | (solenoid_open << sol_ch); 
    SVmutex.give();
};

void StateData::setEmatchState(uint8_t ematch_ch, bool fire_ematch)
{
    EMmutex.take(NEVER);
    // clear then set the bit corresponding to the desired sol_ch (zero indexed)
    ematchstate = (ematchstate & ~(1U << ematch_ch)) | (fire_ematch << ematch_ch); 
    EMmutex.give();
};

float StateData::getSensorState(uint8_t ch_id)
{
    senseMutex.take(NEVER);
    float temp = sensorstate[ch_id];
    senseMutex.give();
    return temp;
};

bool StateData::getSolenoidState(uint8_t sol_ch)
{
    SVmutex.take(NEVER);
    bool temp = (solenoidstate & (0b1 << sol_ch) ) >> sol_ch ;
    SVmutex.give();
    return temp;
};

bool StateData::getEmatchState(uint8_t ematch_ch)
{
    EMmutex.take(NEVER);
    bool temp = (ematchstate & (0b1 << ematch_ch) ) >> ematch_ch;
    EMmutex.give();
    return temp;
};