#include "ADCTask.hpp"

ADCTask::ADCTask(uint8_t priority) : Task(priority, "LED"){};

void ADCTask::activity()
{
    vTaskDelay(6000);
    Serial.println("Starting Now!");

    // Set up adc overall
    sys.adc.begin(8);
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

    bool led = HIGH;
    while(true)
    {
        vTaskDelay(1000);
        led = !led;
        digitalWrite(1, led);
        // TODO: Make this non-blocking.
        sys.adc.waitThenReadData();
        adcdata_t adc_data;
        sys.adc.getData(adc_data.dataword, adc_data.channel);
        Serial.println("Data Channel");
        Serial.println(adc_data.channel);
        Serial.println(adc_data.dataword);

        sys.tasks.sensortask.addADCData(adc_data);
    }
}