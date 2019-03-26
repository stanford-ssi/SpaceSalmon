#include "hal_adc_sync.h"
#include <stdint.h>

class Battery
{
  private:
    adc_sync_descriptor *ADC;
  public:

    struct cell_voltage_t
    {
        float cellA; //this is the lower cell
        float cellB; //this is the upper cell
        float total; //both cells
    };
    
    Battery(adc_sync_descriptor *adc);
    cell_voltage_t readVoltage();
};
