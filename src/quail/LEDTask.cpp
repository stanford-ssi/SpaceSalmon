#include "LEDTask.hpp"

#include "ad7124-lib/ad7124.h"

LEDTask::LEDTask(uint8_t priority) : Task(priority, "LED"){};

void LEDTask::activity()
{
    vTaskDelay(6000);
    Serial.println("Starting Now!");

    Ad7124Chip adc;
    Serial.println(adc.begin(8));



    Serial.println(adc.setConfig(0, Ad7124::RefAVdd, Ad7124::Pga1, false));
    // Setting channel 0 using pins AIN1(+)/AIN0(-)
    Serial.println(adc.setChannel(0, 0, Ad7124::AIN1Input, Ad7124::AVSSInput));
    // Configuring ADC in Full Power Mode (Fastest)
    Serial.println(adc.setAdcControl(Ad7124::StandbyMode, Ad7124::FullPower, true));

    bool led = HIGH;
    uint32_t time = xTaskGetTickCount();
    while (true)
    {
        led = !led;
        digitalWrite(1, led);

        int32_t value = adc.read(0);
        if (value >= 0)
        {
            // If the measurement is successful, the value is converted into voltage
            double voltage = Ad7124Chip::toVoltage(value, 1, 3.6, false);
            // Print result
            voltage = voltage * 1.5;
            double pressure = (voltage - 0.5) /4.0 *1000; 
            Serial.print(value);
            Serial.print("    "); 
            Serial.print(voltage, 3);
            Serial.print("    ");
            Serial.println(pressure, 3);
        }
        else
        {
            Serial.println(value);
            Serial.println("FAIL");
        }
    } 

    
    
    // Serial.println(adc.setConfig(0, Ad7124::RefIn1, Ad7124::Pga16, true));
    // // Setting channel 0 using pins AIN1(+)/AIN0(-)
    // Serial.println(adc.setChannel(0, 0, Ad7124::AIN12Input, Ad7124::AIN0Input));
    // // Configuring ADC in Full Power Mode (Fastest)
    // Serial.println(adc.setAdcControl(Ad7124::StandbyMode, Ad7124::FullPower, true));

    // bool led = HIGH;
    // uint32_t time = xTaskGetTickCount();
    // while (true)
    // {
    //     led = !led;

    //     digitalWrite(1, led);

    //     int32_t value = adc.read(0);
    //     if (value >= 0)
    //     {
    //         // If the measurement is successful, the value is converted into voltage
    //         double voltage = Ad7124Chip::toVoltage(value, 16, 2.5, true);
    //         // Print result
    //         voltage = voltage / 5.0;
    //         double load = voltage / 0.030 * 1000.0;
    //         Serial.print(value);
    //         Serial.print("    "); 
    //         Serial.print(voltage, 5);
    //         Serial.print("    ");
    //         Serial.print(load, 3);
    //         Serial.print("    ");
    //         uint32_t newtime = xTaskGetTickCount();
    //         Serial.println(newtime - time);
    //         time = newtime;
    //     }
    //     else
    //     {
    //         Serial.println(value);
    //         Serial.println("FAIL");
    //     }
    // } 

}