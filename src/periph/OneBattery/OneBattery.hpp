#pragma once

#include "hal_adc_sync.h"
#include <stdint.h>

class OneBattery
{
  private:
    adc_sync_descriptor *ADC;
  public:

    struct cell_voltage_t
    {
        float cellMain;
        float cellBackup;
        float tempJunction;
    };
    
    OneBattery(adc_sync_descriptor *adc);
    cell_voltage_t readVoltage();
};