#include "Battery.hpp"

Battery::Battery(adc_sync_descriptor *adc)
{
    ADC = adc;
}

Battery::cell_voltage_t Battery::readVoltage()
{
    uint16_t vSenseA, vSenseB;

    adc_sync_enable_channel(ADC, 1);

    adc_sync_set_inputs(ADC, 14, 0, 1);
    adc_sync_read_channel(ADC, 1, (uint8_t *)&vSenseA, 2);

    adc_sync_set_inputs(ADC, 13, 0, 1);
    adc_sync_read_channel(ADC, 1, (uint8_t *)&vSenseB, 2);

    float vDivA = 0.6520;
    float vDivB = 0.3256;

    cell_voltage_t result;

    result.cellA = 3.3F * vSenseA / vDivA / 1024;
    result.total = 3.3F * vSenseB / vDivB / 1024;
    result.cellB = result.total - result.cellA;

    return result;
}