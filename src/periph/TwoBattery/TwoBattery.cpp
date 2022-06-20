#include "TwoBattery.hpp"

TwoBattery::TwoBattery(ADC& adc, uint8_t a_ch, uint8_t b_ch): _adc(adc), _a_ch(a_ch), _b_ch(b_ch)
{}

TwoBattery::two_cell_voltage_t TwoBattery::readVoltage()
{
    two_cell_voltage_t result;
    uint16_t raw;
    raw = _adc.read(_a_ch);
    result.cellA = 3.3F * raw / 0.666 / 1024;
    raw = _adc.read(_b_ch);
    result.cellB = (3.3F * raw / 0.333 / 1024) - result.cellA;

    return result;
}