#include "Sensor.hpp"

void Sensor::initialize()
{
    sys.adc.setChannel(this->channel, this->cfg, this->ainp, this->ainm);
}; 