#pragma once

class Sensor;

#include "ad7124-lib/ad7124.h"
#include <cstdint>
#include <string>


class Sensor
{
public:
    Sensor(uint8_t channel, std::string ch_name, Ad7124::InputSel ainp, Ad7124::InputSel ainm)
    : channel(channel), ch_name(ch_name), ainp(ainp), ainm(ainm){}; 

    float convertToFloat(uint32_t adc_dataword){return 0.0;}; //over-written by inheritors, returns SI unit value of reading from ADC bin count
    void initialize(); 

    const uint8_t channel; // ADC channel ID
    const std::string ch_name; // data channel name
    const Ad7124::InputSel ainp; // ADC positive input
    const Ad7124::InputSel ainm; // ADC negative input

protected:
    uint8_t cfg; // ADC channel configuration
};
#include "main.hpp"
