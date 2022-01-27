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
    while(true){
        xEventGroupWaitBits(evgroup, ADC_STARTED, pdFALSE, pdFALSE, NEVER); // wait for ADC to begin (don't clear bits)
        _configure(this); // configure this sensor
        xEventGroupWaitBits(evgroup, ADC_STARTED_DONE, pdFALSE, pdFALSE, NEVER); // wait for all sensors to be reconfigured.
        while(true){
            uint32_t adc_data;
            if(adcbuf.receiveTimeout(adc_data, 100)){ // wait for data to arrive, blocking
                sensor_value = this->convertToFloat(adc_data); // convert data to sensor value in metric unit
                sys.statedata.setSensorState(ch_id, sensor_value); // post new value to state
            }
            if (xEventGroupGetBits(evgroup) & ADC_STARTED){
                break;
            }
        }
    }
}

void Sensor::_configure(Sensor* sensor){
    config_mx.take(NEVER);
    Serial.print("Configured Sensor:");
    Serial.print(num_sens_cfgd);
    Serial.print("/");
    Serial.print(num_sensors);
    Serial.println("");
    if(num_sens_cfgd < num_sensors){
        sensor->configure(); // call the child's implementation of configure()
        num_sens_cfgd++; // record that another sensor has been configuredred    
    }

    if (num_sens_cfgd == num_sensors){
        num_sens_cfgd++;
        sys.tasks.adctask.sensorsConfigured(); // tell ADC task to get goin
        xEventGroupClearBits(evgroup,ADC_STARTED);
        xEventGroupSetBits(evgroup,ADC_STARTED_DONE);
    }

    config_mx.give();
}

void Sensor::ADCbegin(){
    config_mx.take(NEVER);
    num_sens_cfgd = 0;
    Sensor::num_cfgs = 0;
    PressureSensor::unconfigure();
    LoadSensor::unconfigure();
    ThermalSensor::unconfigure();
    config_mx.give();
    xEventGroupClearBits(evgroup, ADC_STARTED_DONE);
    xEventGroupSetBits(evgroup, ADC_STARTED);
}
