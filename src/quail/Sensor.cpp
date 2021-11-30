#include "Sensor.hpp"
#include "main.hpp"

uint8_t Sensor::sensor_priority = SENSOR_PRIORITY; // set default sensor priority
uint8_t Sensor::num_cfgs = 0;
uint8_t Sensor::num_sensors = 0;
uint8_t Sensor::num_sens_cfgd = 0;
StaticEventGroup_t Sensor::evbuf;
EventGroupHandle_t Sensor::evgroup = xEventGroupCreateStatic(&evbuf);
Mutex Sensor::config_mx;

void Sensor::activity(){
    xEventGroupWaitBits(evgroup, ADC_STARTED, pdFALSE, pdTRUE, NEVER); // wait for ADC to begin (don't clear bits)
    _configure(this); // configure this sensor
    while(true){
        uint32_t adc_data;
        adcbuf.receive(adc_data, true); // wait for data to arrive, blocking
        sensor_value = this->convertToFloat(adc_data); // convert data to sensor value in metric unit
        sys.statedata.setSensorState(ch_id, sensor_value); // post new value to state
    }
}

void Sensor::_configure(Sensor* sensor){
    config_mx.take(NEVER);
    sensor->configure(); // call the child's implementation of configure()
    num_sens_cfgd++; // record that another sensor has been configured
    if(num_sens_cfgd == num_sensors) // check if all sensors have been configured
        sys.tasks.adctask.sensorsConfigured(); // tell ADC task to get goin
    config_mx.give();
}

void Sensor::ADCbegin(){
    xEventGroupSetBits(evgroup, ADC_STARTED);
}