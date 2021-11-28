#include "main.hpp"
#include "Sensor.hpp"
#include "main.hpp"

uint8_t Sensor::sensor_priority = SENSOR_PRIORITY; // set default sensor priority
uint8_t Sensor::num_cfgs = 0;
uint8_t Sensor::num_sensors = 0;

void Sensor::activity(){
    _configure(this); // configure this sensor
    while(true){
        uint32_t adc_data;
        adcbuf.receive(adc_data, true); // wait for data to arrive
        sensor_value = this->convertToFloat(adc_data); // convert data to sensor pressure in metric unit
        sys.statedata.setSensorState(ch_id, sensor_value); // post new value to state
        Serial.println(ch_name);
        Serial.println(sensor_value);
    }
}

void Sensor::_configure(Sensor* sensor){
    sensor->configure(); // call the child's implementation of configure()
    num_sens_cfgd++; // record that another sensor has been configured
    if(num_sens_cfgd == num_sensors) // check if all sensors have been configured
        sys.tasks.adctask.sensorsConfigured(); // tell ADC task to get goin
}