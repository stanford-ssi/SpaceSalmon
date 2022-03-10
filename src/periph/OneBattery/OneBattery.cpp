// #include "OneBattery.hpp"

// /*
// TODO:
// -Option for Thunderguppy voltage
// -Measure Chip Temp
// -Measure VBAT
// */

// OneBattery::OneBattery(ADC& adc): _adc(adc)
// {}

// OneBattery::cell_voltage_t OneBattery::readVoltage()
// {
//     uint16_t readx0E, readx19;

//     readx0E = _adc.read(0x0E);
//     readx19 = _adc.read(0x19);

//     cell_voltage_t result;

//     result.cellMain = (((float)readx0E / 1024.) * 4.357) - 0.04793; //This is tuned for TGPY SN. 2 (needs to be reworked)
//     result.cellBackup = 3.3F * 4 * readx19 / 1024;

//     return result;
// }