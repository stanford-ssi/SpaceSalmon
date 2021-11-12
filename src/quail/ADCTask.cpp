#include "ADCTask.hpp"

ADCTask::ADCTask(uint8_t priority) : Task(priority, "LED"){
    evgroup = xEventGroupCreateStatic(&evbuf);
    // Set up adc overall
    sys.adc.begin();
    sys.adc.setAdcControl(Ad7124::ContinuousMode, Ad7124::FullPower, true, Ad7124::InternalClk, true);
    sys.adc.setMode(Ad7124::ContinuousMode);
};


void ADCTask::adcISR(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(sys.tasks.adctask.evgroup, ADC_READY, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void ADCTask::activity()
{
    // Set up configurations
    // PressureSensor::configure();
    //ThermalSensor::configure();
    //LoadSensor::configure();
    
    // attach interrupt to trigger isr function
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