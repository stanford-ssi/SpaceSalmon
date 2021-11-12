#include "PressureSensor.hpp"

uint8_t PressureSensor::cfg = UNCONFIGURED; // initially unconfigured

PressureSensor::PressureSensor(const char* ch_name, Ad7124::InputSel ainp) : Sensor(ch_name, ainp, Ad7124::AVSSInput) {  
    if( cfg != UNCONFIGURED) {
        cfg = Sensor::add_config(); // only add config if this is the first Pressure Sensor
        sys.adc.setConfig(cfg, Ad7124::RefAVdd, Ad7124::Pga1, false);
        sys.adc.setConfigFilter(cfg, Ad7124::Sinc3Filter, 1);
    }
    sys.adc.setChannel(ch_id, cfg, ainp, Ad7124::AVSSInput, true);
    sys.adc.enableChannel(ch_id, true); 
};

void PressureSensor::configure() {
    
};

float PressureSensor::convertToFloat(uint32_t adc_dataword)
{
    float voltage = Ad7124Chip::toVoltage(adc_dataword, 1, 2.5, false);
    voltage = voltage * 1.5;
    //TODO: make this calculation variable based on range, set by this->range
    float pressure = (voltage - 0.5) / 4.0 * 1000;
    return pressure;
};