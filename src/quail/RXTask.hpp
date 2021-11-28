#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <timers.h>
#include <hal_rtos.h>
#include "event_groups.h"
#include "ArduinoJson.h"
#include "Task.hpp"

typedef enum {
    CLOSE_SV = 2, // cmd = 2X - close solenoid on chanel X, zero-indexed
    OPEN_SV = 3, // cmd = 3X - open solenoid on channel X, zero-indexed
    PULSE_SV = 4, // cmd = 4X - pulse solenoid on channel X, zero-indexed
    FIRE_EM = 5, // cmd = 5X - fire ematch on channel X, zero-indexed
    RADIOCONFIG = 6, // cmd = 6X - radio settings update incoming
    ABORTOX = 7, // cmd = 7X - abort ox side only (close fill, open vent, open ox bleed)
    ABORTFUEL = 8, // cmd = 8X - abort fuel side only (close fill, open fuel-side main valve, open fuel bleed)
    ABORT = 9, // cmd = 9X - abort both fuel and ox 
    LAUNCH = 10, // cmd = 10X - closes both fill, waits 10 seconds, then starts igniter; delays another 4 seconds, and opens main valves
    CANCELLAUNCH = 11, // cmd = 11X - cancels launch command
} CMD_PREFIX_t;

class RXTask: public Task<1000>
{
public:
    RXTask(uint8_t priority, uint8_t rx_interval_ms);
    void activity();
private:
    const uint8_t rx_interval_ms; // time to wait before checking command buffer

    TimerHandle_t pulseTimers[8]; // xTimers for callback (timer IDS correspond to the array index & solenoid channel)
    StaticTimer_t pulsebufs[8]; // xTimer static buffer for pulse timers
    // TimerHandle_t launchTimer; // xTimer for launch
    // StaticTimer_t launchbuf; // xTimer static buffer for launch timer

    void process_cmd(uint8_t cmd); // uses the command defs from above to determine what to do with a received command

    // functions used for executing commands
    static void open_solenoid(uint8_t sol_ch); // sets solenoid state to open
    static void close_solenoid(uint8_t sol_ch); // sets solenoid state of given channel to closed
    void pulse_solenoid(uint8_t sol_ch); // reads a time in milliseconds from serial, opens solenoid, sets a timer to close solenoid after specified duration
    static void close_solenoid(TimerHandle_t xTimer); //close the solenoid associated with the given xTimer
    static void fire_ematch(uint8_t em_ch);

    // TODO: implement the following
    // void config_radio(); // reads radio settings from serial and sends to radio settings msgbuffer
    // void abort_fuel(); // aborts fuel-side 
    // void abort_ox(); // aborts ox-side
    // void launch(); // starts launch sequence
    // static void fire_igniter(TimerHandle_t xTimer); // fires engine igniter ematches
    // static void open_main_valves(TimerHandle_t xTimer); // opens main engine valves
    // void cancel_launch(); // cancels the launch sequence
};