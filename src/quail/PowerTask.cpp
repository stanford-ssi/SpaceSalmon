#include "PowerTask.hpp"
#include "main.hpp"

PowerTask::PowerTask(uint8_t priority)
    : Task(priority, "PowerMonitor"),
      watchdog(sys.telem_slate.watchdog_counter, sys.telem_slate.watchdog_control){};

void PowerTask::activity()
{
    TickType_t time = xTaskGetTickCount();
    uint8_t i = 0;

    while (true)
    {
        xTaskDelayUntil(&time, 100);
        uint16_t i_sense = adc0.read(0x03); // ADC0/AIN[2] is PB09
        // 0.002 volts per amp
        // x100 = 0.2 volts per amp post amplification
        // 3.3V / 0.2 = 16.5A full scale
        // ADC is in 10 bit mode -> divide by 1024
        float i_batt = ((float)i_sense) / (1024 / 16.5); // amps
        sys.telem_slate.i_batt.set(i_batt);
        uint16_t v_sense = adc0.read(0x02); // ADC0/AIN[3] is PB08
        // Using a voltage divider
        // R2 = 43kΩ
        // R4 = 10kΩ
        // V_batt = (43000 + 10000) / 10000 * V_sense
        float v_batt = ((float)v_sense) / (1024 / 3.3) * 53 / 10;
        sys.telem_slate.v_batt.set(v_batt);

        if (v_batt > 11.0)
        {
            digitalWrite(PWR_LED, 1);
        }
        else
        {
            digitalWrite(PWR_LED, 0);
        }

        digitalWrite(ACT_LED, i % 2);
        i++;

        watchdog.service();
    }
}