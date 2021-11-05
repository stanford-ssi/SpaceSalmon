#pragma once

class System;

#include <Arduino.h>

#include "SPI.h"
#include "ad7124-lib/ad7124.h"

#include "ADCTask.hpp"
#include "StateTask.hpp"
//#include "TelemTask.hpp"
//#include "RadioTask.hpp"
//#include "USBTask.hpp"
//#include "CmdTask.hpp"
//#include "SolenoidTask.hpp"
#include "FireTask.hpp"
#include "SensorTask.hpp"


class System
{ 
public:
    SPIClass adc_spi = SPIClass(&sercom1, 5, 6, 7, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);
    SPIClass squib_spi = SPIClass(&sercom0, 17, 18, 19, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);

    Ad7124Chip adc;
    
    class Tasks
    {
    public:
        ADCTask adctask = ADCTask(2); //reads from ADC
        //FireTask firetask = FireTask(3); //fires MC33797
        StateTask statetask = StateTask(1); //collects & logs state information
        //TelemTask telemtask = TelemTask(3); // passes state data to output (TX)
        //USBTask usbtask = USBTask(4); //collects and sends information over USB
        //RadioTask radiotask = RadioTask(4); //collects and sends information over radio
        //CmdTask cmdtask = CmdTask(3); //processes commands from radio/usb (RX)
        SensorTask sensortask = SensorTask(1); //processes ADC values depending on sensor type
    };

    Tasks tasks;
};

#include "main.hpp"