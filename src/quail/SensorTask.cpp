#include "SensorTask.hpp"

SensorTask::SensorTask(uint8_t priority) : Task(priority, "Sensor"){};

Sensor* SensorTask::sensors[NUM_SENSORS] = 
{
    // new PressureSensor("PT1", Ad7124::AIN1Input) // channel, ADC in port
};

void SensorTask::activity() {
    while(true) {
        adcdata_t adcpacket;
        adcbuf.receive(adcpacket, NEVER); // wait until an adc packet is ready, state goes BLOCKED until not empty
        Sensor* cur_sensor = sensors[adcpacket.channel]; // sensor for which we just received data
        float val = cur_sensor->convertToFloat(adcpacket.dataword); // convert ADC packet from 24bit to float
        sensordata_t sensedata;
        sensedata.ch_id = cur_sensor->ch_name;
        sensedata.value = val;
        //sys.tasks.statetask.updateSensorState(sensedata);
        Serial.println(cur_sensor->ch_id);
        Serial.println(val, 5);
    }
};

void SensorTask::addADCData(adcdata_t adc_data){
    adcbuf.send(adc_data);
};
