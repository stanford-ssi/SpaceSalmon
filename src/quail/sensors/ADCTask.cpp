#include "ADCTask.hpp"
#include "main.hpp"
#include "wiring_private.h"

ADCTask::ADCTask(uint8_t priority) : Task(priority, "LED"){
    evgroup = xEventGroupCreateStatic(&evbuf);
}

void ADCTask::adcISR(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(sys.tasks.adctask.evgroup, NewData, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void ADCTask::activity()
{
    while(true) {
        initADC();
        while(err_count + timeout_count < ERROR_THRESHOLD) {
            EventBits_t flags = xEventGroupWaitBits(evgroup, NewData, true, false, READ_TIMEOUT);
            if (!(flags & NewData)) { // time out, likely temperature or power problems
                timeout_count++;
                // char msg_buffer[50];
                // sprintf(msg_buffer, "ADC Timed out %i times", timeout_count);
                // sys.tasks.txtask.writeUSB(msg_buffer);
                continue; // skip data processing
            }

            adcdata_t curr_data;
            sys.adc.clearIRQAction();
            long ret = sys.adc.getData(curr_data.dataword, curr_data.channel);
            if(ret >= 0) { // process data
                sys.adc.setIRQAction(adcISR);
                if(curr_data.channel < Sensor::numSensors()) {
                    sys.tasks.sensortask.addADCdata(curr_data);
                    data_count++;
                    if(data_count >= Sensor::numSensors()) { // read through all sensor
                        data_count = 0;
                        sys.tasks.sensortask.dataReady(); 
                    }
                } else { // out-of-boundary data
                    // char msg_buffer[50];
                    // sprintf(msg_buffer, "Got some OOB data on ch:%i", curr_data.channel);
                    // sys.tasks.txtask.writeUSB(msg_buffer);
                }
            } else { // failed ADC read, check checksum and SPI
                uint32_t error_reg = sys.adc.getRegister(Ad7124::RegisterId::Error);
                sys.adc.setIRQAction(adcISR);

                uint32_t error_mask = AD7124_ERR_REG_ADC_CONV_ERR |
                                      AD7124_ERR_REG_AINM_UV_ERR |
                                      AD7124_ERR_REG_AINM_OV_ERR |
                                      AD7124_ERR_REG_AINP_UV_ERR |
                                      AD7124_ERR_REG_AINP_OV_ERR;

                if(error_reg & ~error_mask){ // if there was any error not in the mask
                    // It was some other (more serious) error
                    err_count++;
                    // char msg_buffer[50];
                    // sprintf(msg_buffer, "ADC error: %li (%i time)", error_reg, err_count);
                    // sys.tasks.txtask.writeUSB(msg_buffer);
                }else{
                    // It was an input error:
                    // char msg_buffer[50];
                    // sprintf(msg_buffer, "Conversion error %li on channel %i, probably railed!", error_reg, curr_data.channel);
                    // sys.tasks.txtask.writeUSB(msg_buffer);
                }
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
        char msg_buffer[50];
        sprintf(msg_buffer, "ADC Init Failed: %i", ret);
        sys.tasks.txtask.writeUSB(msg_buffer);
        vTaskDelay(100);
    }

    sys.adc.setAdcControl(Ad7124::ContinuousMode, Ad7124::FullPower, true, Ad7124::InternalClk, true);
    sys.adc.setMode(Ad7124::ContinuousMode);
    SensorTask::initSensors(); // tell sensors that they are ready for configuration

    // Wait for sensors to be configured!
    xEventGroupWaitBits(evgroup, SensorsReady, true, false, NEVER);
    
    sys.adc.setRegister(Ad7124::Error_En, AD7124_ERREN_REG_SPI_CRC_ERR_EN |
                                            AD7124_ERREN_REG_SPI_IGNORE_ERR_EN |
                                            AD7124_ERREN_REG_ADC_SAT_ERR_EN |
                                            AD7124_ERREN_REG_ADC_CAL_ERR_EN |
                                            //AD7124_ERREN_REG_ADC_CONV_ERR_EN |
                                            AD7124_ERREN_REG_REF_DET_ERR_EN |
                                            //AD7124_ERREN_REG_AINM_OV_ERR_EN |
                                            //AD7124_ERREN_REG_AINM_UV_ERR_EN |
                                            //AD7124_ERREN_REG_AINP_OV_ERR_EN |
                                            //AD7124_ERREN_REG_AINP_UV_ERR_EN |
                                            AD7124_ERREN_REG_ALDO_PSM_ERR_EN |
                                            AD7124_ERREN_REG_DLDO_PSM_ERR_ERR );

    sys.adc.setIRQAction(adcISR);
}

void ADCTask::resetADC() {
    timeout_count = 0;
    err_count = 0;
    data_count = 0;
    sys.adc.reset();
    sys.tasks.txtask.writeUSB("Resetting ADC!");
}

void ADCTask::sensorsConfigured(){
    xEventGroupSetBits(evgroup, SensorsReady);
}