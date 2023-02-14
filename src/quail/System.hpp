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
#include "PowerTask.hpp"
#include "rx_tx_log/LoggerTask.hpp"
#include "rx_tx_log/TXTask.hpp"

#include "rx_tx_log/CmdServer.hpp"
#include "rx_tx_log/SlateServer.hpp"
#include "generated/telemetry.h"
#include "config.h"
#include "rx_tx_log/SlateRegistry.hpp"
#include "actuators/ValveTask.hpp"

class System
{ 
public:
    SPIClass adc_spi = SPIClass(&sercom1, 5, 6, 7, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);
    SPIClass squib_spi = SPIClass(&sercom0, 17, 18, 19, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);

    Ad7124Chip adc = Ad7124Chip(5,8,&adc_spi);

    SSIEth ethernet = SSIEth(1, MacAddr({0x00, 0x00, 0x00, 0x00, 0x20, 0x76}));

    telemetry_t telem_slate;
    SlateServer<telemetry_t> telem_server = SlateServer(telem_slate);

    SlateRegistry<telemetry_t> slate_registry = SlateRegistry<telemetry_t>(telem_server);

    LoadSensor LC1 = LoadSensor(Ad7124::AIN12Input, telem_slate.lc1);
    LoadSensor LC2 = LoadSensor(Ad7124::AIN13Input, telem_slate.lc2);
    PressureSensor PT1 = PressureSensor(Ad7124::AIN1Input, RANGE_2500, telem_slate.oxdz_source_press);
    PressureSensor PT2 =  PressureSensor(Ad7124::AIN2Input, RANGE_1000, telem_slate.oxdz_up_press);
    PressureSensor PT3 =  PressureSensor(Ad7124::AIN3Input, RANGE_2500, telem_slate.oxdz_down_press);
    PressureSensor PT4 =  PressureSensor(Ad7124::AIN4Input, RANGE_1000, telem_slate.pt4);
    PressureSensor PT5 =  PressureSensor(Ad7124::AIN5Input, RANGE_1000, telem_slate.fuel_source_press);
    PressureSensor PT6 =  PressureSensor(Ad7124::AIN6Input, RANGE_1000, telem_slate.fuel_up_press);
    PressureSensor PT7 =  PressureSensor(Ad7124::AIN7Input, RANGE_1000, telem_slate.chamber_press);
    PressureSensor PT8 =  PressureSensor(Ad7124::AIN8Input, RANGE_1000, telem_slate.pt8);
    ThermalSensor TS1 = ThermalSensor(Ad7124::AIN9Input, telem_slate.tc1); //samd51 # defines TC0-7 so don't use those
    ThermalSensor TS2 = ThermalSensor(Ad7124::AIN10Input, telem_slate.tc2);
    //ThermalSensor TS3 = ThermalSensor(Ad7124::AIN11Input, telem_slate.tc3);

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
        PowerTask powertask = PowerTask(3); // task for measuring battery voltage and current
        
        CmdServer cmdserver = CmdServer(4);

        TXTask txtask = TXTask(6, 50); //regularly collects state data, logs and sends over USB, radio, or ethernet
        LoggerTask logger = LoggerTask(1); // logs data to SD during idle time, writes USB data as available
        ValveTask valves = ValveTask(5);

    } tasks = Tasks(*this);
};

#include "main.hpp"