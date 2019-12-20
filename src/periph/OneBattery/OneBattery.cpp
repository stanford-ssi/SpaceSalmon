#include "OneBattery.hpp"

/*
TODO:
-Option for Thunderguppy voltage
-Measure Chip Temp
-Measure VBAT
*/

OneBattery::OneBattery(ADC& adc): _adc(adc)
{}

OneBattery::cell_voltage_t OneBattery::readVoltage()
{
    uint16_t readx0D, readx19;

    readx0D = _adc.read(0x0D);
    readx19 = _adc.read(0x19);

    cell_voltage_t result;

    result.cellMain = 3.3F * readx0D / 0.6520 / 1024;
    result.cellBackup = 3.3F * 4 * readx19 / 1024;

    return result;
}