#pragma once

#include "ssi_adc.h"
#include <stdint.h>

class TwoBattery
{
  private:
    ADC& _adc;
    uint8_t _a_ch, _b_ch;
  public:

    struct two_cell_voltage_t
    {
        float cellA;
        float cellB;
    };
    
    TwoBattery(ADC& adc, uint8_t a_ch, uint8_t b_ch);
    two_cell_voltage_t readVoltage();
};