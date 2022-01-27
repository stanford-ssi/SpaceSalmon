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
    int i = 0;

    while(true){

        // Set up adc overall
        while(true){
            int ret = sys.adc.begin();
            if(ret == 0){
                break;
            }
            Serial.print("ADC Init Failed:");
            Serial.println(ret);
            vTaskDelay(100);
        }

        sys.adc.setAdcControl(Ad7124::ContinuousMode, Ad7124::FullPower, true, Ad7124::InternalClk, true);
        sys.adc.setMode(Ad7124::ContinuousMode);

        Sensor::ADCbegin(); // tell sensors that they are ready for configuration

        // Wait for sensors to be configured!
        xEventGroupWaitBits(evgroup, SENSORS_READY, true, false, NEVER);
        sys.adc.setIRQAction(adcISR);


        uint32_t timeout_count = 0;
        uint32_t err_count = 0;
        while(true)
        {
            //Serial.println("loop");
            // long err = sys.adc.checkForErrors();
            // char buf[50];
            // sprintf(buf, "Error Code: %l", err);
            // Serial.println(buf);

            if(err_count + timeout_count > 10){
                timeout_count = 0;
                err_count = 0;
                sys.adc.reset();
                Serial.println("Reseting ADC!");
                break;
            }

            // wait for ADC ready
            EventBits_t flags = xEventGroupWaitBits(evgroup, ADC_READY, true, false, 100);

            if (!(flags & ADC_READY)){ //if timed out
                timeout_count++;
                char str[50];
                sprintf(str,"ADC Timed out %i times", timeout_count);
                sys.tasks.txtask.writeUSB(str);
                continue;
            }

            // turn off interrupt to read data
            sys.adc.clearIRQAction();
            
            // read data
            adcdata_t adc_data;
            long ret = sys.adc.getData(adc_data.dataword, adc_data.channel);
            // turn interrupt back on to catch new ready indicator
            sys.adc.setIRQAction(adcISR);

            if( ret >= 0) {
                // do thing with data
                if(i > 1000){
                    Serial.println("Got some good data!");
                    i = 0;
                }
                i++;
                if(adc_data.channel >= Sensor::num_sensors){
                    Serial.print("Got some OOB data on ch: ");
                    Serial.println(adc_data.channel);
                }else{
                    (sys.sensors[adc_data.channel])->addADCdata(adc_data.dataword);
                }
                
            }else{
                err_count++;
                char str[50];
                sprintf(str,"ADC data error %i (%i time)", ret, err_count);
                sys.tasks.txtask.writeUSB(str);
            }

        }    
    }
}

void ADCTask::sensorsConfigured(){
    xEventGroupSetBits(evgroup, SENSORS_READY);
};