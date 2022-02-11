#include "LoadSensor.hpp"
#include "main.hpp"

uint8_t LoadSensor::cfg = UNCONFIGURED; // initially unconfigured

LoadSensor::LoadSensor(char* ch_name, Ad7124::InputSel ainp) : Sensor(ch_name, ainp, Ad7124::AIN0Input) {};

void LoadSensor::configure() {
    if(cfg == UNCONFIGURED){
        cfg = addConfig();
        sys.adc.setConfig(cfg, Ad7124::RefIn1, Ad7124::Pga16, true);
        sys.adc.setConfigFilter(cfg, Ad7124::Sinc3Filter, 10);
    }
    sys.adc.setChannel(ch_id, cfg, ainp, ainm, true);
    sys.adc.enableChannel(ch_id, true);
}

float LoadSensor::convertToFloat(uint32_t adc_dataword)
{
    double voltage = Ad7124Chip::toVoltage(adc_dataword, 16, 2.5, true);
    voltage = voltage / 5.0;
    //TODO: make this calculation variable based on range, set by this->range
    float load = voltage / 0.030 * 1000.0 * 4.4482216153; // return load in N
    return load;
};