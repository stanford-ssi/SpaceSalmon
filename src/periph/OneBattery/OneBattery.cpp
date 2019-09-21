#include "Battery.hpp"

/*
TODO:
-Option for Thunderguppy voltage
-Measure Chip Temp
-Measure VBAT
*/

Battery::Battery(adc_sync_descriptor *adc)
{
    ADC = adc;
}

Battery::cell_voltage_t Battery::readVoltage()
{
    uint16_t readx0E, readx19, readx1C;

    adc_sync_enable_channel(ADC, 1);

    adc_sync_set_inputs(ADC, 0x0E, 0, 1);
    adc_sync_read_channel(ADC, 1, (uint8_t *)&readx0E, 2);

    adc_sync_set_inputs(ADC, 0x19, 0, 1);
    adc_sync_read_channel(ADC, 1, (uint8_t *)&readx19, 2);

    adc_sync_set_inputs(ADC, 0x1C, 0, 1);
    adc_sync_read_channel(ADC, 1, (uint8_t *)&readx1C, 2);

    float vDivA = 0.6520;

    cell_voltage_t result;

    result.cellMain = 3.3F * readx0E / vDivA / 1024;
    result.cellBackup = 3.3F * readx19 / 1024;
    result.tempJunction = readx1C / 256.0F;

    return result;
}