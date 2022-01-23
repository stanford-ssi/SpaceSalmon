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
    // Set up adc overall
    sys.adc.begin();
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
        // wait for ADC ready
        EventBits_t flags = xEventGroupWaitBits(evgroup, ADC_READY, true, false, 100);

        if (!(flags & ADC_READY)){ //if timed out
            timeout_count++;
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
            (sys.sensors[adc_data.channel])->addADCdata(adc_data.dataword);
        }else{
            err_count++;
        }
    }
}

void ADCTask::sensorsConfigured(){
    xEventGroupSetBits(evgroup, SENSORS_READY);
};