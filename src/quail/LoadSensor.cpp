#include "LoadSensor.hpp"

void LoadSensor::configure() {
    cfg = add_config();

    sys.adc.setConfig(cfg, Ad7124::RefIn1, Ad7124::Pga16, true);
    sys.adc.setConfigFilter(cfg, Ad7124::Sinc3Filter, 10);
}

LoadSensor::LoadSensor(char* ch_name, Ad7124::InputSel ainp, Ad7124::InputSel ainm) : Sensor(ch_name, ainp, ainm) {
    sys.adc.setChannel(ch_id, cfg, ainp, ainm, true);
    sys.adc.enableChannel(ch_id, true);
};

float LoadSensor::convertToFloat(uint32_t adc_dataword)
{
    double voltage = Ad7124Chip::toVoltage(adc_dataword, 1, 2.5, false);
    voltage = voltage * 1.5;
    //TODO: make this calculation variable based on range, set by this->range
    float load = (voltage - 0.5) / 4.0 * 1000;
    return load;
};