#include "ADCTask.hpp"

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
    sys.adc.setTimeout(1); // set IO timeout to be 1ms

    vTaskDelay(2000); // put this task to sleep for a bit to allow sensors time to configure & initialize 
    sys.adc.setIRQAction(adcISR);
    while(true)
    {
        // wait for ADC ready
        uint32_t flags = xEventGroupWaitBits(evgroup, ADC_READY, true, false, NEVER);
        // turn off interrupt to read data
        sys.adc.clearIRQAction();
        // read data
        adcdata_t adc_data;
        sys.adc.getData(adc_data.dataword, adc_data.channel);
        // turn interrupt back on to catch new ready indicator
        sys.adc.setIRQAction(adcISR);
        // do thing with data
        Serial.println("Data Channel");
        Serial.println(adc_data.channel);
        Serial.println(adc_data.dataword);
        (sys.sensors[adc_data.channel])->addADCdata(adc_data.dataword);
        //sys.tasks.sensortask.addADCData(adc_data);
    }
}