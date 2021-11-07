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

    // //Channel 0: Load Cell 1
    // Serial.println(adc.setConfig(0, Ad7124::RefIn1, Ad7124::Pga16, true));
    // adc.setConfigFilter(0, Ad7124::Sinc3Filter, 10);
    // Serial.println(adc.setChannel(0, 0, Ad7124::AIN12Input, Ad7124::AIN0Input));

    // //Channel 1: PT1
    // Serial.println(adc.setConfig(1, Ad7124::RefIn2, Ad7124::Pga1, false));
    // adc.setConfigFilter(1, Ad7124::Sinc3Filter, 10);
    // Serial.println(adc.setChannel(1, 1, Ad7124::AIN1Input, Ad7124::AVSSInput, true));

    // //Channel 2: TC1
    // Serial.println(adc.setConfig(2, Ad7124::RefAVdd, Ad7124::Pga1, true));
    // adc.setConfigFilter(2, Ad7124::Sinc3Filter, 10);
    // Serial.println(adc.setChannel(2, 2, Ad7124::AIN8Input, Ad7124::AIN0Input, true));

    // //Channel 3: PT3
    // Serial.println(adc.setConfig(3, Ad7124::RefAVdd, Ad7124::Pga1, false));
    // adc.setConfigFilter(3, Ad7124::Sinc3Filter, 1);
    // Serial.println(adc.setChannel(3, 3, Ad7124::AIN3Input, Ad7124::AVSSInput, true));

    // //Channel 4: PT4
    // Serial.println(adc.setConfig(4, Ad7124::RefAVdd, Ad7124::Pga1, false));
    // adc.setConfigFilter(4, Ad7124::Sinc3Filter, 1);
    // Serial.println(adc.setChannel(4, 4, Ad7124::AIN4Input, Ad7124::AVSSInput, true));

    // //Channel 5: PT5
    // Serial.println(adc.setConfig(5, Ad7124::RefAVdd, Ad7124::Pga1, false));
    // adc.setConfigFilter(5, Ad7124::Sinc3Filter, 1);
    // Serial.println(adc.setChannel(5, 5, Ad7124::AIN5Input, Ad7124::AVSSInput, true));

    // //Why do we actually need this? why only 0-7?
    // adc.enableChannel(0, true);
    // adc.enableChannel(1, true);
    // adc.enableChannel(2, true);
    // adc.enableChannel(3, true);
    // adc.enableChannel(4, true);
    // adc.enableChannel(5, true);
    // adc.enableChannel(6, false);
    // adc.enableChannel(7, false);

    // //adc.setTimeout(1);
    

    // bool led = HIGH;
    // uint32_t time = xTaskGetTickCount();
    // while (true)
    // {
    //     led = !led;
    //     digitalWrite(1, led);

    //     uint32_t SBraw[6];

    //     for (int i = 1; i <= 6; i++)
    //     {
    //         adc.waitThenReadData();
    //         uint32_t dataWord;
    //         uint8_t channel;
    //         adc.getData(dataWord,channel);
    //         SBraw[channel] = dataWord;
    //         // Serial.print(channel);
    //         // Serial.print(" : ");
    //         // Serial.println(dataWord);
    //     }

    //     // double voltage = Ad7124Chip::toVoltage(SBraw[0], 16, 2.5, true);
    //     // // Print result
    //     // voltage = voltage / 5.0;
    //     // double load = voltage / 0.030 * 1000.0;
    //     // // Serial.print("LC1: ");
    //     // // Serial.print(SBraw[0]);
    //     // // Serial.print("    ");
    //     // // Serial.print(voltage, 5);
    //     // // Serial.print("    ");
    //     // Serial.println(load, 3);

    //     // double voltage = Ad7124Chip::toVoltage(SBraw[1], 1, 2.5, false);
    //     // voltage = voltage * 1.5;
    //     // double pressure = (voltage - 0.5) / 4.0 * 1000;
    //     // Serial.print("PT1: ");
    //     // Serial.print(SBraw[1]);
    //     // Serial.print("    ");
    //     // Serial.print(voltage, 3);
    //     // Serial.print("    ");
    //     // Serial.println(pressure, 3);

    //     double voltage = Ad7124Chip::toVoltage(SBraw[2], 1, 3.6, true);
    //     double temp = voltage / 0.005; // 5mV per degree C
    //     // Serial.print("TC1: ");
    //     // Serial.print(SBraw[2]);
    //     // Serial.print("    ");
    //     // Serial.print(voltage, 3);
    //     // Serial.print("    ");
    //     Serial.println(temp, 3);

    //     // uint32_t newtime = xTaskGetTickCount();
    //     // Serial.println(newtime - time);
    //     // time = newtime;
    // }
}