#pragma once

class System;

#include <Arduino.h>

#include "SPI.h"
#include "ad7124-lib/ad7124.h"

#define RADIO_TXRX // uncomment this to use Radio for TX/RX
#define SENSOR_PRIORITY 3 // set sensor priority

#include "PressureSensor.hpp"
// #include "ThermalSensor.hpp"
#include "LoadSensor.hpp"
#include "ValveTask.hpp"
#include "ADCTask.hpp"
#include "StateData.hpp"
#include "FireTask.hpp"
#include "LoggerTask.hpp"
#include "TXTask.hpp"
#include "RXTask.hpp"
#include "SequenceTask.hpp""
#ifdef RADIO_TXRX
    #include "RadioTask.hpp"
#endif

class System
{ 
public:
    SPIClass adc_spi = SPIClass(&sercom1, 5, 6, 7, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);
    SPIClass squib_spi = SPIClass(&sercom0, 17, 18, 19, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);

    Ad7124Chip adc = Ad7124Chip(5,8,&adc_spi);

    PressureSensor PT1 = PressureSensor("PT1", Ad7124::AIN1Input, RANGE_1000);
    PressureSensor PT2 =  PressureSensor("PT2", Ad7124::AIN2Input, RANGE_1000);
    // PressureSensor PT3 =  PressureSensor("PT3", Ad7124::AIN2Input, RANGE_1000);
    // PressureSensor PT4 =  PressureSensor("PT4", Ad7124::AIN3Input, RANGE_1000);
    // PressureSensor PT5 =  PressureSensor("PT5", Ad7124::AIN4Input, RANGE_1000);
    LoadSensor LC1 = LoadSensor("LC1", Ad7124::AIN12Input);
    // LoadSensor LC2 = LoadSensor("LC2", Ad7124::AIN7Input);

    Sensor* sensors [3] = {
        &PT1,
        &PT2,
        // &PT3,
        // &PT4,
        // &PT5,
        &LC1,
        // &LC2
    };

    StateData statedata = StateData(); //holds current state of sensors/SVs/ematches systems for output + control
    
    class Tasks
    {
    public:
        ADCTask adctask = ADCTask(2); // passes ADC raw data to the appropriate sensor
        ValveTask valvetask = ValveTask(6, 22); // controls solenoids 
        FireTask firetask = FireTask(6, 20, 21); //fires squibs for ematches
        #ifdef RADIO_TXRX // if using radio, create a RadioTask
            RadioTask radiotask = RadioTask(2); //collects and sends information over radio
        #endif
        TXTask txtask = TXTask(4, 1000); //regularly collects state data, logs and sends over USB or radio
        RXTask rxtask = RXTask(5, 100); //processes commands from USB or radio
        LoggerTask logger = LoggerTask(1); // logs data to SD during idle time, writes USB data as available
        SequenceTask sequencetask = SequenceTask(10); // test task for debugging
    };

    Tasks tasks;
};

#include "main.hpp"