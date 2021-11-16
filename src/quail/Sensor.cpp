#include "main.hpp"
#include "Sensor.hpp"
#include "main.hpp"

uint8_t Sensor::sensor_priority = 2;
uint8_t Sensor::num_cfgs = 0;
uint8_t Sensor::num_sensors = 0;

void Sensor::activity(){
    this->configure(); // configure this sensor
    while(true){
        uint32_t adc_data;
        adcbuf.receive(adc_data, true); // wait for data to arrive
        sensor_value = this->convertToFloat(adc_data); // convert data to sensor pressure in metric unit
        sys.tasks.statetask.setSensorState(ch_id, sensor_value); // post new value to state
        Serial.println(ch_name);
        Serial.println(sensor_value);
    }
}

void Sensor::initialized(){
    num_sens_cfgd++;
    if(num_sens_cfgd == num_sensors)
        sys.tasks.adctask.sensorsConfigured();
}