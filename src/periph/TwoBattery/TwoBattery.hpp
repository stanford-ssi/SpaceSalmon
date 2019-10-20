#include "hal_adc_sync.h"
#include <stdint.h>

class TwoBattery
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
    
    TwoBattery(adc_sync_descriptor *adc);
    cell_voltage_t readVoltage();
};