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

    // Set up configurations
    // Initialize pressure configuration
    sys.adc.setConfig(PRESSURE_CFG, Ad7124::RefIn2, Ad7124::Pga1, false);
    sys.adc.setConfigFilter(PRESSURE_CFG, Ad7124::Sinc3Filter, 10);

    // Initialize load configuration
    sys.adc.setConfig(LOAD_CFG, Ad7124::RefIn1, Ad7124::Pga16, true);
    sys.adc.setConfigFilter(LOAD_CFG, Ad7124::Sinc3Filter, 10);

    // Initialize thermal configuration
    sys.adc.setConfig(THERMAL_CFG, Ad7124::RefAVdd, Ad7124::Pga1, true);
    sys.adc.setConfigFilter(THERMAL_CFG, Ad7124::Sinc3Filter, 10);

    // Set up sensors
    SensorTask::initialize(); // static function initializes ADC channels for Sensor list in SensorTask.cpp

    // Set adc timeout
    sys.adc.setTimeout(1);

    
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
        sys.adc.setIRQAction(adcISR);
        // do thing with data
        Serial.println("Data Channel");
        Serial.println(adc_data.channel);
        Serial.println(adc_data.dataword);
        //sys.tasks.sensortask.addADCData(adc_data);
    }
}