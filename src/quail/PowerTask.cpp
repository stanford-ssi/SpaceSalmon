#include "PowerTask.hpp"
#include "main.hpp"

PowerTask::PowerTask(uint8_t priority) : Task(priority, "LED"){
    evgroup = xEventGroupCreateStatic(&evbuf);
};

void PowerTask::activity()
{
    while(true){
        uint16_t i_meas = adc0.read(0x03); // ADC0/AIN[2] is PB09
        // 0.002 volts per amp
        // x100 = 0.2 volts per amp post amplification
        // 3.3V / 0.2 = 16.5A full scale
        float i_batt = i_meas / (UINT16_MAX / 16.5); // amps
        sys.statedata.setBatteryCurrentState(i_batt);
        uint16_t v_meas = adc0.read(0x03); // ADC0/AIN[3] is PB08
        // Using a voltage divider
        // R2 = 43kΩ
        // R4 = 10kΩ
        // V_batt = (43000 + 10000) / 10000 * V_meas
        uint16_t v_batt = 5.3 * v_meas;
        sys.statedata.setBatteryCurrentState(v_batt);

    }
}