#pragma once

class System;

#include <Arduino.h>

#include <Adafruit_SleepyDog.h>
#include "SPI.h"
#include "sensors/ad7124-lib/ad7124.h"

#include "sensors/Sensor.hpp"
#include "sensors/PressureSensor.hpp"
#include "sensors/ThermalSensor.hpp"
#include "sensors/LoadSensor.hpp"
#include "sensors/SensorTask.hpp"
#include "sensors/ADCTask.hpp"
#include "sensors/PowerTask.hpp"
#include "actuators/ValveTask.hpp"
#include "actuators/FireTask.hpp"
#include "rx_tx_log/LoggerTask.hpp"
#include "rx_tx_log/TXTask.hpp"
#include "rx_tx_log/RXTask.hpp"
#include "fsms/EngineFSM.hpp"
#include "fsms/TankFSM.hpp"
#include "fsms/VaPakFSM.hpp"
#ifdef ETHERNET_TXRX
    #include "rx_tx_log/EthernetTask.hpp"
#endif
#ifdef RADIO_TXRX
    #include "rx_tx_log/RadioTask.hpp"
#endif 
#include "slate/Slate.hpp"
#include "config.h"

class System
{ 
public:
    SPIClass adc_spi = SPIClass(&sercom1, 5, 6, 7, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);
    SPIClass squib_spi = SPIClass(&sercom0, 17, 18, 19, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);

    Ad7124Chip adc = Ad7124Chip(5,8,&adc_spi);

    #ifdef ETHERNET_TXRX
        SSIEth ethernet = SSIEth(1);
    #endif

    Slate slate = Slate("quail");

    LoadSensor LC1 = LoadSensor(Ad7124::AIN12Input, slate.sense.lc1);
    LoadSensor LC2 = LoadSensor(Ad7124::AIN13Input, slate.sense.lc2);
    PressureSensor PT1 = PressureSensor(Ad7124::AIN1Input, RANGE_2500, slate.sense.pt1);
    PressureSensor PT2 =  PressureSensor(Ad7124::AIN2Input, RANGE_1000, slate.sense.pt2);
    PressureSensor PT3 =  PressureSensor(Ad7124::AIN3Input, RANGE_2500, slate.sense.pt3);
    PressureSensor PT4 =  PressureSensor(Ad7124::AIN4Input, RANGE_1000, slate.sense.pt4);
    PressureSensor PT5 =  PressureSensor(Ad7124::AIN5Input, RANGE_1000, slate.sense.pt5);
    PressureSensor PT6 =  PressureSensor(Ad7124::AIN6Input, RANGE_1000, slate.sense.pt6);
    PressureSensor PT7 =  PressureSensor(Ad7124::AIN7Input, RANGE_1000, slate.sense.pt7);
    PressureSensor PT8 =  PressureSensor(Ad7124::AIN8Input, RANGE_1000, slate.sense.pt8);
    ThermalSensor TS1 = ThermalSensor(Ad7124::AIN9Input, slate.sense.tc1); //samd51 # defines TC0-7 so don't use those
    ThermalSensor TS2 = ThermalSensor(Ad7124::AIN10Input, slate.sense.tc2);
    //ThermalSensor TS3 = ThermalSensor(Ad7124::AIN11Input, slate.sense.tc3);

    Sensor* sensors [12] = {
        &LC1,
        &LC2,
        &PT1,
        &PT2,
        &PT3,
        &PT4,
        &PT5,
        &PT6,
        &PT7,
        &PT8,
        &TS1,
        &TS2,
        //&TS3,
    };

    struct Tasks{
        Tasks(System& sys) : sys(sys) {};
        System& sys;

        ADCTask adctask = ADCTask(2); // passes ADC raw data to the appropriate sensor
        SensorTask sensortask = SensorTask(3);
        PowerTask powertask = PowerTask(3); // test for measuring battery voltage and current
        
        ValveTask valvetask = ValveTask(6, 22); // controls solenoids 
        FireTask firetask = FireTask(6, 20); //fires squibs for ematches
        
        #ifdef RADIO_TXRX // if using radio, create a RadioTask
            RadioTask radiotask = RadioTask(4); //collects and sends information over radio
        #endif
        #ifdef ETHERNET_TXRX
            EthernetTask ethernettask = EthernetTask(4);
        #endif
        TXTask txtask = TXTask(5, 50); //regularly collects state data, logs and sends over USB, radio, or ethernet
        RXTask rxtask = RXTask(5, 50); //processes commands from USB, radio, or ethernet
        LoggerTask logger = LoggerTask(1); // logs data to SD during idle time, writes USB data as available
    
        // EngineFSM engine = EngineFSM(2);
        // // VaPakFSM oxTank = VaPakFSM(2,
        // VaPakFSM oxTank = VaPakFSM(2,
        //     sys.slate.sequence.oxState,
        //     sys.slate.sequence.oxOpWeight,
        //     sys.slate.sense.pt4,
        //     // sys.slate.sense.lcs,
        //     sys.slate.valves[4],
        //     sys.slate.valves[7],
        //     sys.slate.valves[5]
        // );
        // TankFSM fuelTank = TankFSM(2,
        //     sys.slate.sequence.fuelState,
        //     sys.slate.sequence.fuelOpPressure,
        //     sys.slate.sense.pt2,
        //     sys.slate.valves[2], // fill
        //     sys.slate.valves[0], // use main as vent
        //     sys.slate.valves[3] // bleed
        // );
    } tasks = Tasks(*this);
};

#include "main.hpp"