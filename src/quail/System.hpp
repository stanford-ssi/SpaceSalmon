#pragma once

class System;

#include <Arduino.h>

#include "SPI.h"
#include "ad7124-lib/ad7124.h"


//#include "Sensor.hpp"
#include "PressureSensor.hpp"
// #include "ThermalSensor.hpp"
//#include "LoadSensor.hpp"
#include "ADCTask.hpp"
#include "StateData.hpp"
#include "LoggerTask.hpp"
//#include "TelemTask.hpp"
//#include "RadioTask.hpp"
//#include "USBTask.hpp"
//#include "CmdTask.hpp"
//#include "SolenoidTask.hpp"
#include "FireTask.hpp"

// #define RADIO_TXRX // uncomment this to use Radio for TX/RX

class System
{ 
public:
    SPIClass adc_spi = SPIClass(&sercom1, 5, 6, 7, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);
    SPIClass squib_spi = SPIClass(&sercom0, 17, 18, 19, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);

    Ad7124Chip adc = Ad7124Chip(5,8,&adc_spi);

    //PressureSensor PT0 = PressureSensor("PT0", Ad7124::AIN0Input, RANGE_1000);
    // PressureSensor PT1 =  PressureSensor("PT1", Ad7124::AIN1Input, RANGE_1000);
    // PressureSensor PT2 =  PressureSensor("PT2", Ad7124::AIN2Input, RANGE_1000);
    // PressureSensor PT3 =  PressureSensor("PT3", Ad7124::AIN3Input, RANGE_1000);
    // PressureSensor PT4 =  PressureSensor("PT4", Ad7124::AIN4Input, RANGE_1000);
    // LoadSensor LC0 = LoadSensor("LC0", Ad7124::AIN5Input);
    // LoadSensor LC1 = LoadSensor("LC1", Ad7124::AIN7Input);

    Sensor* sensors [0] = {
        // &PT0,
        // &PT1,
        // &PT2,
        // &PT3,
        // &PT4,
        // &LC0,
        // &LC1
    };

    StateData statedata = StateData(); //holds current state of sensors/SVs/ematches systems for output + control
    
    class Tasks
    {
    public:
        ADCTask adctask = ADCTask(3); // passes ADC raw data to the appropriate sensor
        //SolenoidTask soltask = SolenoidTask(3); //updates solenoid states 
        FireTask firetask = FireTask(3, 20, 21); //fires squibs for ematches
        #ifdef RADIO_TXRX
        RadioTask radiotask = RadioTask(2); //collects and sends information over radio
        #endif
        //TXTask txtask = TXTask(2, 10); //regularly collects state data, logs and sends over USB or radio
        //RXTask rxtask = RXTask(3); //processes commands from USB or radio
        LoggerTask logger = LoggerTask(1); // logs data to SD during idle time, writes USB data as available
    };

    Tasks tasks;
};

#include "main.hpp"