#include "PressureSensor.hpp"
#include "main.hpp"

uint8_t PressureSensor::cfg = UNCONFIGURED; // this sensor type is initially unconfigured

PressureSensor::PressureSensor(const char* ch_name, Ad7124::InputSel ainp, PressureRange range) : 
    Sensor(ch_name, ainp, Ad7124::AVSSInput), range(range) {};

void PressureSensor::configure() {
    if(cfg == UNCONFIGURED) { // cfg is static - once one sensor of a type is set up, this loop won't run
        cfg = add_config();
        sys.adc.setConfig(cfg, Ad7124::RefIn2, Ad7124::Pga1, false);
        sys.adc.setConfigFilter(cfg, Ad7124::Sinc3Filter, 512);
    }
    sys.adc.setChannel(ch_id, cfg, ainp, ainm, true);
    sys.adc.enableChannel(ch_id, true); 
};

float PressureSensor::convertToFloat(uint32_t adc_dataword)
{
    float voltage = Ad7124Chip::toVoltage(adc_dataword, 1, 2.5, false);
    voltage = voltage * 1.5;
    //TODO: make this calculation variable based on range, set by this->range
    float pressure = (voltage - 0.5) / 4.0 * (range*PSI_TO_PA);
    return pressure;
};