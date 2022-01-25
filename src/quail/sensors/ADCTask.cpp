#include "ADCTask.hpp"
#include "main.hpp"
#include "wiring_private.h"

ADCTask::ADCTask(uint8_t priority) : Task(priority, "LED"){
    evgroup = xEventGroupCreateStatic(&evbuf);
};

void ADCTask::adcISR(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(sys.tasks.adctask.evgroup, ADC_READY, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void ADCTask::activity()
{
    while(true){
        // Set up adc overall
        sys.adc.begin();
        sys.adc.setAdcControl(Ad7124::ContinuousMode, Ad7124::FullPower, true, Ad7124::InternalClk, true);
        sys.adc.setMode(Ad7124::ContinuousMode);
        SensorTask::ADCbegin(); // tell sensors that they are ready for configuration

        // Wait for sensors to be configured!
        xEventGroupWaitBits(evgroup, SENSORS_READY, true, false, NEVER);
        sys.adc.setIRQAction(adcISR);
        uint32_t timeout_count = 0;
        uint32_t err_count = 0;
        uint8_t data_count = 0;
        while(true)
        {
            // wait for ADC ready
            EventBits_t flags = xEventGroupWaitBits(evgroup, ADC_READY, true, false, 100);

            if (!(flags & ADC_READY)){ //if timed out
                timeout_count++;
                char str[50];
                sprintf(str,"ADC Timed out %i times", timeout_count);
                sys.tasks.txtask.writeUSB(str);
                continue;
            }

            data_count++;
            
            // turn off interrupt to read data
            sys.adc.clearIRQAction();
            
            // read data
            adcdata_t adc_data;
            long ret = sys.adc.getData(adc_data.dataword, adc_data.channel);
            // turn interrupt back on to catch new ready indicator
            sys.adc.setIRQAction(adcISR);
            if( ret >= 0) {
                // do thing with data
                sys.tasks.sensortask.addADCdata(adc_data);
            }else{
                err_count++;
                char str[50];
                sprintf(str,"ADC data error %i times", err_count);
                sys.tasks.txtask.writeUSB(str);
            }

            if(data_count >= Sensor::numSensors()){
                data_count = 0;
                sys.tasks.sensortask.dataReady(); // ready for some conversions!
            }

            if(err_count + timeout_count > 10){
                sys.adc.reset();
                break;
            }
        }    
    }
}

void ADCTask::sensorsConfigured(){
    xEventGroupSetBits(evgroup, SENSORS_READY);
};