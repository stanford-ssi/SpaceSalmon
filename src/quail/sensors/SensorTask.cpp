#include "SensorTask.hpp"
#include "main.hpp"

StaticEventGroup_t SensorTask::evbuf;
EventGroupHandle_t SensorTask::evgroup = xEventGroupCreateStatic(&evbuf);

void SensorTask::activity() {
    while (true) {
        xEventGroupWaitBits(evgroup, NEED_CONFIG, pdTRUE, pdFALSE, NEVER); // wait for ADC to begin (don't clear bits)
        configureSensors();
        while (!(xEventGroupGetBits(evgroup) & NEED_CONFIG)) { // read data until ADC restart
            if(xEventGroupGetBits(evgroup) & DATA_READY) {
                readData();
            }
        }
    }
}

void SensorTask::configureSensors() {
    for (uint8_t i = 0; i < Sensor::numSensors(); i++) {
        sys.sensors[i]->configure();
    }
    sys.tasks.adctask.sensorsConfigured(); // let ADC know sensors are configured
    xEventGroupClearBits(evgroup, NEED_CONFIG); // clear for next reset
}

void SensorTask::initSensors() {
    PressureSensor::unconfigure();
    LoadSensor::unconfigure();
    ThermalSensor::unconfigure();
    xEventGroupSetBits(evgroup, NEED_CONFIG);
}

bool SensorTask::readData() {
    adcdata_t adc_data;
    for (uint8_t i = 0; i < Sensor::numSensors(); i++) {
        if(adcbuf.receiveTimeout(adc_data, READ_TIMEOUT)) {
            float sensor_value = sys.sensors[adc_data.channel]->convertToFloat(adc_data.dataword); // convert data to metric unit
            sys.statedata.setSensorState(adc_data.channel, sensor_value); // post new value to state
        } else {
            sys.tasks.txtask.writeUSB("Insufficient data for read");
            return false;
        }
    }
    return true;
}

void SensorTask::dataReady() {
    xEventGroupSetBits(evgroup, DATA_READY);
}
