#pragma once

class System;

#include <Arduino.h>

#include "SPI.h"
#include "sensors/ad7124-lib/ad7124.h"

//#define RADIO_TXRX // uncomment this to use Radio for TX/RX

#include "sensors/Sensor.hpp"
#include "sensors/PressureSensor.hpp"
#include "sensors/ThermalSensor.hpp"
#include "sensors/LoadSensor.hpp"
#include "sensors/SensorTask.hpp"
#include "sensors/ADCTask.hpp"
#include "actuators/ValveTask.hpp"
#include "actuators/FireTask.hpp"
#include "rx_tx_log/LoggerTask.hpp"
#include "rx_tx_log/TXTask.hpp"
#include "rx_tx_log/RXTask.hpp"
#ifdef RADIO_TXRX
    #include "rx_tx_log/RadioTask.hpp"
#endif 
#include "SequenceLauncher.hpp"
#include "PowerTask.hpp"
#include "slate/Slate.hpp"
#include "config.h"

class System
{ 
public:
    SPIClass adc_spi = SPIClass(&sercom1, 5, 6, 7, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);
    SPIClass squib_spi = SPIClass(&sercom0, 17, 18, 19, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);

    Ad7124Chip adc = Ad7124Chip(5,8,&adc_spi);

<<<<<<< HEAD
    Slate slate = Slate("quail");
=======
    PressureSensor PT1 =  PressureSensor("PT1", Ad7124::AIN1Input, RANGE_2500);
    PressureSensor PT2 =  PressureSensor("PT2", Ad7124::AIN2Input, RANGE_1000);
    PressureSensor PT3 =  PressureSensor("PT3", Ad7124::AIN3Input, RANGE_2500);
    PressureSensor PT4 =  PressureSensor("PT4", Ad7124::AIN4Input, RANGE_1000);
    PressureSensor PT5 =  PressureSensor("PT5", Ad7124::AIN5Input, RANGE_1000);
    PressureSensor PT6 =  PressureSensor("PT6", Ad7124::AIN6Input, RANGE_1000);
    PressureSensor PT7 =  PressureSensor("PT7", Ad7124::AIN7Input, RANGE_1000);
    LoadSensor LC1 = LoadSensor("LC1", Ad7124::AIN12Input);
    LoadSensor LC2 = LoadSensor("LC2", Ad7124::AIN13Input);
    ThermalSensor TS1 = ThermalSensor("TC1", Ad7124::AIN8Input); //samd51 # defines TC0-7 so don't use those
    ThermalSensor TS2 = ThermalSensor("TC2", Ad7124::AIN9Input);    
>>>>>>> origin/ethernet

    PressureSensor PT1 = PressureSensor(Ad7124::AIN1Input, RANGE_1000, slate.sense.pt1);
    PressureSensor PT2 =  PressureSensor(Ad7124::AIN2Input, RANGE_1000, slate.sense.pt2);
    PressureSensor PT3 =  PressureSensor(Ad7124::AIN3Input, RANGE_1000, slate.sense.pt3);
    PressureSensor PT4 =  PressureSensor(Ad7124::AIN4Input, RANGE_1000, slate.sense.pt4);
    PressureSensor PT5 =  PressureSensor(Ad7124::AIN4Input, RANGE_1000, slate.sense.pt5);
    PressureSensor PT6 =  PressureSensor(Ad7124::AIN4Input, RANGE_1000, slate.sense.pt6);
    PressureSensor PT7 =  PressureSensor(Ad7124::AIN4Input, RANGE_1000, slate.sense.pt7);
    LoadSensor LC1 = LoadSensor(Ad7124::AIN12Input, slate.sense.lc1);
    LoadSensor LC2 = LoadSensor(Ad7124::AIN7Input, slate.sense.lc2);
    ThermalSensor TS1 = ThermalSensor(Ad7124::AIN8Input, slate.sense.tc1); //samd51 # defines TC0-7 so don't use those
    ThermalSensor TS2 = ThermalSensor(Ad7124::AIN8Input, slate.sense.tc2); //samd51 # defines TC0-7 so don't use those

    Sensor* sensors [11] = {
        &PT1,
        &PT2,
        &PT3,
        &PT4,
        &PT5,
        &PT6,
        &PT7,
        &LC1,
        &LC2,
        &TS1,
        &TS2
    };

    class Tasks
    {
    public:
        // ADCTask adctask = ADCTask(2); // passes ADC raw data to the appropriate sensor
        // SensorTask sensortask = SensorTask(3);
        // PowerTask powertask = PowerTask(3); // test for measuring battery voltage and current
        
        // ValveTask valvetask = ValveTask(6, 22); // controls solenoids 
        // FireTask firetask = FireTask(6, 20, 21); //fires squibs for ematches
        // SequenceLauncher seqlauncher  = SequenceLauncher(3);
        
        #ifdef RADIO_TXRX // if using radio, create a RadioTask
            RadioTask radiotask = RadioTask(2); //collects and sends information over radio
        #endif
        TXTask txtask = TXTask(5, 50); //regularly collects state data, logs and sends over USB or radio
        // RXTask rxtask = RXTask(5, 50); //processes commands from USB or radio
        // LoggerTask logger = LoggerTask(1); // logs data to SD during idle time, writes USB data as available
    };

    Tasks tasks;
};

#include "main.hpp"