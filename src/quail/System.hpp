#pragma once

class System;

#include <Arduino.h>

#include "SPI.h"
#include "ad7124-lib/ad7124.h"

// #include "StateTask.hpp"
#include "PressureSensor.hpp"
//#include "ThermalSensor.hpp"
//#include "LoadSensor.hpp"
#include "ADCTask.hpp"
//#include "TelemTask.hpp"
//#include "RadioTask.hpp"
//#include "USBTask.hpp"
//#include "CmdTask.hpp"
//#include "SolenoidTask.hpp"
//#include "FireTask.hpp"


class System
{ 
public:
    SPIClass adc_spi = SPIClass(&sercom1, 5, 6, 7, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);
    SPIClass squib_spi = SPIClass(&sercom0, 17, 18, 19, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);

    Ad7124Chip adc = Ad7124Chip(5,8,&adc_spi);

    PressureSensor PT1 = PressureSensor("PT1",Ad7124::AIN0Input);

    Sensor* sensors [1] = {
        &PT1
    };
    
    class Tasks
    {
    public:
        ADCTask adctask = ADCTask(2); //reads from ADC
        //FireTask firetask = FireTask(3); //fires squibs for ematches
        //SolenoidTask soltask = SolenoidTask(3); //sets solenoid states
        //StateTask statetask = StateTask(1); //collects & logs state information
        //TelemTask telemtask = TelemTask(3, TELEM_MODE); // passes state data to output (TX) based on mode selection
            //USBTask usbtask = USBTask(4); //collects and sends information over USB
            //RadioTask radiotask = RadioTask(4); //collects and sends information over radio
        //CmdTask cmdtask = CmdTask(3); //processes commands from radio/usb (RX)
        //SensorTask sensortask = SensorTask(1); //processes ADC values depending on sensor type
    };

    Tasks tasks;
};

#include "main.hpp"