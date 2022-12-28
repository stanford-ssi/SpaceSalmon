#include "Sensor.hpp"
#include "main.hpp"

uint8_t Sensor::num_sensors = 0;
uint8_t Sensor::num_cfgs = 0;

Sensor::Sensor(Ad7124::InputSel ainp, Ad7124::InputSel ainm, SlateField<float>& slate) : 
ch_id(num_sensors), ainp(ainp), ainm(ainm), slate(slate)
{num_sensors++;} 