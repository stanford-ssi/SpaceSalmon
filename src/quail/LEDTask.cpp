#include "LEDTask.hpp"

#include "ad7124-lib/ad7124.h"

LEDTask::LEDTask(uint8_t priority) : Task(priority, "LED"){};

void LEDTask::activity()
{
    vTaskDelay(6000);
    Serial.println("Starting Now!");

    Ad7124Chip adc;
    Serial.println(adc.begin(8));

    Serial.println(adc.setConfig(0, Ad7124::RefInternal, Ad7124::Pga1, true));
    // Setting channel 0 using pins AIN1(+)/AIN0(-)
    Serial.println(adc.setChannel(0, 0, Ad7124::AIN1Input, Ad7124::AIN2Input));
    // Configuring ADC in Full Power Mode (Fastest)
    Serial.println(adc.setAdcControl(Ad7124::StandbyMode, Ad7124::FullPower, true));

    while (true)
    {
        digitalWrite(1, HIGH);
        vTaskDelay(100);
        digitalWrite(1, LOW);

        int32_t value = adc.read(0);
        if (value >= 0)
        {
            // If the measurement is successful, the value is converted into voltage
            double voltage = Ad7124Chip::toVoltage(value, 8, 2.5, true);
            // Print result
            Serial.println(voltage, 3);
        }
        else
        {
            Serial.println(value);
            Serial.println("FAIL");
        }
    }
}