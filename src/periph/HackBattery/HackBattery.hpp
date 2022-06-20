#pragma once

#include "ssi_adc.h"
#include <stdint.h>

class HackBattery
{
  private:
    ADC& _adc;
    uint8_t _ch;
  public:

    struct one_cell_voltage_t
    {
        float cell;
    };
    
    HackBattery(ADC& adc, uint8_t ch);
    one_cell_voltage_t readVoltage();
};