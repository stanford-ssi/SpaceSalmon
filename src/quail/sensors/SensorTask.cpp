#include "SensorTask.hpp"
#include "Sensor.hpp"
#include "main.hpp"

StaticEventGroup_t SensorTask::evbuf;
EventGroupHandle_t SensorTask::evgroup = xEventGroupCreateStatic(&evbuf);

SensorTask::SensorTask(uint8_t priority) : Task(priority, "SensorTask") {}

void SensorTask::activity() {
    while (true) {
        xEventGroupWaitBits(evgroup, NEED_CONFIG, pdTRUE, pdFALSE, NEVER); // wait for ADC to begin (don't clear bits)
        configureSensors();
        while (true) { // read data until ADC restart
            uint32_t flags = xEventGroupWaitBits(evgroup, NEED_CONFIG | DATA_READY, false, false, NEVER);
            if (flags & NEED_CONFIG){
                break;
            } else {
                readData();
            }
        }
    }
}

void SensorTask::configureSensors() {
    PressureSensor::unconfigure();
    LoadSensor::unconfigure();
    ThermalSensor::unconfigure();
    for (uint8_t i = 0; i < Sensor::numSensors(); i++) {
        sys.sensors[i]->configure();
    }
    sys.tasks.adctask.sensorsConfigured(); // let ADC know sensors are configured
    xEventGroupClearBits(evgroup, NEED_CONFIG); // clear for next reset
}

void SensorTask::initSensors() {
    xEventGroupSetBits(evgroup, NEED_CONFIG);
}

bool SensorTask::readData() {
    ADCTask::adcdata_t adc_data;
    while(adcbuf.receiveTimeout(adc_data, READ_TIMEOUT)) {
        sys.sensors[adc_data.channel]->convertToFloat(adc_data.dataword); // convert data to metric unit
    }
    xEventGroupClearBits(evgroup, DATA_READY);
    return true;
}

void SensorTask::dataReady() {
    xEventGroupSetBits(evgroup, DATA_READY);
}
