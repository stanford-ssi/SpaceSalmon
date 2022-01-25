#include "SensorTask.hpp"
#include "main.hpp"

StaticEventGroup_t SensorTask::evbuf;
EventGroupHandle_t SensorTask::evgroup = xEventGroupCreateStatic(&evbuf);

void SensorTask::activity(){
    xEventGroupWaitBits(evgroup, ADC_STARTED, pdTRUE, pdFALSE, NEVER); // wait for ADC to begin (don't clear bits)
    for(uint8_t i = 0; i < Sensor::numSensors(); i++){
        sys.sensors[i]->configure();
    }
    sys.tasks.adctask.sensorsConfigured();
    while(true){
        uint32_t flags = xEventGroupWaitBits(evgroup, ADC_RESET|DATA_READY, pdTRUE, pdFALSE, NEVER); // await a reset or a data ready flag from ADC
        if(flags & ADC_RESET)
            xEventGroupWaitBits(evgroup, ADC_STARTED, pdTRUE, pdFALSE, NEVER);
        else if(flags & DATA_READY){ // if got data ready flag, clear buf and update sensor values
            while(!adcbuf.empty()){
                adcdata_t adc_data;
                bool ret = adcbuf.receive(adc_data, true); // wait for data to arrive, blocking     
                if(ret){
                    float sensor_value = sys.sensors[adc_data.channel]->convertToFloat(adc_data.dataword); // convert data to sensor value in metric unit
                    sys.statedata.setSensorState(adc_data.channel, sensor_value); // post new value to state
                }
            }
        }
    }
}

void SensorTask::ADCbegin(){
    xEventGroupSetBits(evgroup, ADC_STARTED);
}

void SensorTask::ADCreset(){
    xEventGroupSetBits(evgroup, ADC_RESET);
}

void SensorTask::dataReady(){
    xEventGroupSetBits(evgroup, DATA_READY);
}
