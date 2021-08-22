#include "LEDTask.hpp"

#include "ad7124-lib/ad7124.h"

LEDTask::LEDTask(uint8_t priority) : Task(priority, "LED"){};

void LEDTask::activity()
{
    vTaskDelay(6000);
    Serial.println("Starting Now!");

    Ad7124Chip adc;
    Serial.println(adc.begin(8));

    adc.setAdcControl(Ad7124::ContinuousMode, Ad7124::FullPower, true, Ad7124::InternalClk, true);
    adc.setMode(Ad7124::ContinuousMode);

    //Channel 0: Load Cell 1
    Serial.println(adc.setConfig(0, Ad7124::RefIn1, Ad7124::Pga16, true));
    adc.setConfigFilter(0, Ad7124::Sinc3Filter, 1);
    Serial.println(adc.setChannel(0, 0, Ad7124::AIN12Input, Ad7124::AIN0Input));

    //Channel 1: PT1
    Serial.println(adc.setConfig(1, Ad7124::RefAVdd, Ad7124::Pga1, false));
    adc.setConfigFilter(1, Ad7124::Sinc3Filter, 1);
    Serial.println(adc.setChannel(1, 1, Ad7124::AIN1Input, Ad7124::AVSSInput, true));

    //Channel 2: PT2
    Serial.println(adc.setConfig(2, Ad7124::RefAVdd, Ad7124::Pga1, false));
    adc.setConfigFilter(2, Ad7124::Sinc3Filter, 1);
    Serial.println(adc.setChannel(2, 2, Ad7124::AIN2Input, Ad7124::AVSSInput, true));

    //Channel 3: PT3
    Serial.println(adc.setConfig(3, Ad7124::RefAVdd, Ad7124::Pga1, false));
    adc.setConfigFilter(3, Ad7124::Sinc3Filter, 1);
    Serial.println(adc.setChannel(3, 3, Ad7124::AIN3Input, Ad7124::AVSSInput, true));

    //Channel 4: PT4
    Serial.println(adc.setConfig(4, Ad7124::RefAVdd, Ad7124::Pga1, false));
    adc.setConfigFilter(4, Ad7124::Sinc3Filter, 1);
    Serial.println(adc.setChannel(4, 4, Ad7124::AIN4Input, Ad7124::AVSSInput, true));

    //Channel 5: PT5
    Serial.println(adc.setConfig(5, Ad7124::RefAVdd, Ad7124::Pga1, false));
    adc.setConfigFilter(5, Ad7124::Sinc3Filter, 1);
    Serial.println(adc.setChannel(5, 5, Ad7124::AIN5Input, Ad7124::AVSSInput, true));

    //Why do we actually need this? why only 0-7?
    adc.enableChannel(0, true);
    adc.enableChannel(1, true);
    adc.enableChannel(2, true);
    adc.enableChannel(3, true);
    adc.enableChannel(4, true);
    adc.enableChannel(5, true);
    adc.enableChannel(6, false);
    adc.enableChannel(7, false);

    //adc.setTimeout(1);
    

    bool led = HIGH;
    uint32_t time = xTaskGetTickCount();
    while (true)
    {
        led = !led;
        digitalWrite(1, led);

        uint32_t SBraw[6];

        for (int i = 1; i <= 6; i++)
        {
            adc.waitThenReadData();
            uint32_t dataWord;
            uint8_t channel;
            adc.getData(dataWord,channel);
            SBraw[channel] = dataWord;
            Serial.print(channel);
            Serial.print(" : ");
            Serial.println(dataWord);
        }

        double voltage = Ad7124Chip::toVoltage(SBraw[0], 16, 2.5, true);
        // Print result
        voltage = voltage / 5.0;
        double load = voltage / 0.030 * 1000.0;
        Serial.print("LC1: ");
        Serial.print(SBraw[0]);
        Serial.print("    ");
        Serial.print(voltage, 5);
        Serial.print("    ");
        Serial.println(load, 3);

        voltage = Ad7124Chip::toVoltage(SBraw[1], 1, 3.6, false);
        // Print result
        voltage = voltage * 1.5;
        double pressure = (voltage - 0.5) / 4.0 * 1000;
        Serial.print("PT1: ");
        Serial.print(SBraw[1]);
        Serial.print("    ");
        Serial.print(voltage, 3);
        Serial.print("    ");
        Serial.println(pressure, 3);

        uint32_t newtime = xTaskGetTickCount();
        Serial.println(newtime - time);
        time = newtime;
    }
}