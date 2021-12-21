#include "ThermalSensor.hpp"
#include "main.hpp"
uint8_t ThermalSensor::cfg = UNCONFIGURED; // this sensor type is initially unconfigured

ThermalSensor::ThermalSensor(const char* ch_name, Ad7124::InputSel ainp) : 
    Sensor(ch_name, ainp, Ad7124::AIN0Input){};

void ThermalSensor::configure() {
    if(cfg == UNCONFIGURED) { // cfg is static - once one sensor of a type is set up, this loop won't run
        cfg = add_config();
        sys.adc.setConfig(cfg, Ad7124::RefAVdd, Ad7124::Pga1, false);
        sys.adc.setConfigFilter(cfg, Ad7124::Sinc3Filter, 1);
    }
    sys.adc.setChannel(ch_id, cfg, ainp, ainm, true);
    sys.adc.enableChannel(ch_id, true); 
};

float ThermalSensor::convertToFloat(uint32_t adc_dataword)
{
    float voltage = Ad7124Chip::toVoltage(adc_dataword, 1, 2.5, false);
    voltage = voltage * 2;
    //TODO: make this calculation variable based on range, set by this->range
    float temp = (voltage);
    return temp;
};