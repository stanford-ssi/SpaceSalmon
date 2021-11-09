#pragma once

class Sensor;

#include "ad7124-lib/ad7124.h"
#include <cstdint>
#include <cstring>


class Sensor
{
public:
    Sensor(uint8_t channel, const char* ch_id, Ad7124::InputSel ainp, Ad7124::InputSel ainm)
    : channel(channel), ainp(ainp), ainm(ainm)
    {strncpy(this->ch_id, ch_id, strlen(this->ch_id));}; 

    virtual float convertToFloat(uint32_t adc_dataword); //over-written by inheritors, returns SI unit value of reading from ADC bin count
    void initialize(); 
    char ch_id[5]; // data channel name

protected:
    uint8_t cfg; // ADC channel configuration
    const uint8_t channel; // ADC channel ID
    const Ad7124::InputSel ainp; // ADC positive input
    const Ad7124::InputSel ainm; // ADC negative input
};
#include "main.hpp"
