#include "LoadSensor.hpp"

uint8_t LoadSensor::cfg = UNCONFIGURED; // initially unconfigured

LoadSensor::LoadSensor(char* ch_name, Ad7124::InputSel ainp) : Sensor(ch_name, ainp, Ad7124::AIN0Input) {};

void LoadSensor::configure() {
    if(cfg == UNCONFIGURED){
        cfg = add_config();
        sys.adc.setConfig(cfg, Ad7124::RefIn1, Ad7124::Pga16, true);
        sys.adc.setConfigFilter(cfg, Ad7124::Sinc3Filter, 10);
    }
    sys.adc.setChannel(ch_id, cfg, ainp, ainm, true);
    sys.adc.enableChannel(ch_id, true);
}

float LoadSensor::convertToFloat(uint32_t adc_dataword)
{
    // TODO - make this actually right for load cell conversion!
    double voltage = Ad7124Chip::toVoltage(adc_dataword, 1, 2.5, false);
    voltage = voltage / 5.0;
    //TODO: make this calculation variable based on range, set by this->range
    float load = voltage / 0.030 * 1000.0;
    return load;
};