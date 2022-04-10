#include "ThermalSensor.hpp"
#include "main.hpp"
uint8_t ThermalSensor::cfg = UNCONFIGURED; // this sensor type is initially unconfigured

ThermalSensor::ThermalSensor(Ad7124::InputSel ainp, SensorSlate &slate) : 
    Sensor(slate.id.c_str(), ainp, Ad7124::AIN0Input, slate){};

void ThermalSensor::configure() {
    if(cfg == UNCONFIGURED) { // cfg is static - once one sensor of a type is set up, this loop won't run
        cfg = addConfig();
        sys.adc.setConfig(cfg, Ad7124::RefAVdd, Ad7124::Pga1, true);
        sys.adc.setConfigFilter(cfg, Ad7124::Sinc3Filter, 10);
    }
    sys.adc.setChannel(ch_id, cfg, ainp, ainm, true);
    sys.adc.enableChannel(ch_id, true);
};

void ThermalSensor::convertToFloat(uint32_t adc_dataword)
{
    float voltage = Ad7124Chip::toVoltage(adc_dataword, 1, 3.6, true);
    //TODO: make this calculation variable based on range, set by this->range
    float temp = (voltage * 200.0) + 273.15; // 5mV/degK
    slate.val << temp;

    updateSlate();
};