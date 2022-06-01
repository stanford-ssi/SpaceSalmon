#include "HackBattery.hpp"

HackBattery::HackBattery(ADC& adc, uint8_t ch): _adc(adc), _ch(ch)
{}

HackBattery::one_cell_voltage_t HackBattery::readVoltage()
{
    one_cell_voltage_t result;
    uint16_t raw;
    raw = _adc.read(_ch);
    result.cell = 3.3F * raw / 0.333 / 1024;

    return result;
}