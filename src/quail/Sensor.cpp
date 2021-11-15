#include "Sensor.hpp"

uint8_t Sensor::sensor_priority = 2;
uint8_t Sensor::num_cfgs = 0;
uint8_t Sensor::num_sensors = 0;

void Sensor::activity(){
    this->configure(); // configure yourself
    while(true){
        uint32_t adc_data;
        adcbuf.receive(adc_data, true); // wait for data to arrive
        sensor_value = this->convertToFloat(adc_data); 
        Serial.println(ch_name);
        Serial.println(sensor_value);
    }
}