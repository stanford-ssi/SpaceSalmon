#include "ADCTask.hpp"
#include "main.hpp"
#include "wiring_private.h"

ADCTask::ADCTask(uint8_t priority) : Task(priority, "LED"){
    evgroup = xEventGroupCreateStatic(&evbuf);
}

void ADCTask::adcISR(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(sys.tasks.adctask.evgroup, NEW_DATA, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void ADCTask::activity()
{
    // vTaskDelay(4000); // just for debugging
    while(true) {
        initADC();
        while(err_count + timeout_count < ERROR_THRESHOLD) {
            EventBits_t flags = xEventGroupWaitBits(evgroup, NEW_DATA, true, false, READ_TIMEOUT);
            if (!(flags & NEW_DATA)) { // time out, likely temperature or power problems
                timeout_count++;
                sprintf(msg_buffer, "ADC Timed out %i times", timeout_count);
                sys.tasks.txtask.writeUSB(msg_buffer);
                continue; // skip data processing
            }

            long ret = getData();
            if(ret >= 0) { // process data
                if(curr_data.channel < Sensor::numSensors()) {
                    sys.tasks.sensortask.addADCdata(curr_data);
                    data_count++;
                    if(data_count == Sensor::numSensors()) { // read through all sensors
                        data_count = 0;
                        sys.tasks.sensortask.dataReady(); 
                    }
                } else { // out-of-boundary data
                    sprintf(msg_buffer, "Got some OOB data on ch:%i", curr_data.channel);
                    sys.tasks.txtask.writeUSB(msg_buffer);
                }
            } else { // failed ADC read, check checksum and SPI
                err_count++;
                sprintf(msg_buffer, "ADC data error %i (%i time)", ret, err_count);
                sys.tasks.txtask.writeUSB(msg_buffer);
            }
        }
        resetADC();
    }
}

void ADCTask::initADC() {
    while(true) { // Set up ADC overall
        int ret = sys.adc.begin();
        if(ret == 0) {
            break;
        }
        Serial.print("ADC Init Failed:");
        Serial.println(ret);
        vTaskDelay(100);
    }

    sys.adc.setAdcControl(Ad7124::ContinuousMode, Ad7124::FullPower, true, Ad7124::InternalClk, true);
    sys.adc.setMode(Ad7124::ContinuousMode);
    SensorTask::initSensors(); // tell sensors that they are ready for configuration
    
    // Wait for sensors to be configured!
    xEventGroupWaitBits(evgroup, SENSORS_READY, true, false, NEVER);
    sys.adc.setIRQAction(adcISR);
}

void ADCTask::resetADC() {
    timeout_count = 0;
    err_count = 0;
    data_count = 0;
    sys.adc.reset();
    sys.tasks.txtask.writeUSB("Resetting ADC!");
}

long ADCTask::getData() {
    sys.adc.clearIRQAction();
    long ret = sys.adc.getData(curr_data.dataword, curr_data.channel);
    sys.adc.setIRQAction(adcISR);
    return ret;
}

void ADCTask::sensorsConfigured(){
    xEventGroupSetBits(evgroup, SENSORS_READY);
}