#include "PressureSensor.hpp"
#include "LoadSensor.hpp"
#include "ThermalSensor.hpp"

#include "SensorTask.hpp"

SensorTask::SensorTask(uint8_t priority) : Task(priority, "Sensor"){};

Sensor SensorTask::sensors[NUM_SENSORS] = 
{
    PressureSensor(0, "PT1", Ad7124::AIN1Input,PressureSensor::RANGE_1000) // channel, ADC in port, PT range
};

void SensorTask::activity()
{
    while(true)
    {
        adcdata_t adcpacket;
        adcbuf.receive(adcpacket, NEVER); // wait until an adc packet is ready, state goes BLOCKED until not empty
        Sensor cur_sensor = sensors[adcpacket.channel]; // sensor for which we just received data
        float val = cur_sensor.convertToFloat(adcpacket.dataword); // convert ADC packet from 24bit to float
        sensordata_t sensedata;
        sensedata.ch_name = cur_sensor.ch_name;
        sensedata.value = val;
        sys.tasks.statetask.updateSensorState(sensedata);
        Serial.println(cur_sensor.ch_name.c_str());
        Serial.println(val, 5);
    }
};

void SensorTask::initialize()
{
    for(uint8_t i = 0; i < NUM_SENSORS; i++)
        sensors[i].initialize();
    for(uint8_t i = 0; i < NUM_SENSORS; i++)
        sys.adc.enableChannel(i);
};

void SensorTask::addADCData(adcdata_t adc_data){
    adcbuf.send(adc_data);
};
