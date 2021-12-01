#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "MsgBuffer.hpp"
#include <semphr.h>
#include <timers.h>
#include <hal_rtos.h>
#include "event_groups.h"
#include "ArduinoJson.h"
#include "Task.hpp"

#define MAX_CMD_LENGTH 10 // maximum length of a command, in bytes
#define CMD_ENDLINE 0x0A // endline indicator for a command - null char

typedef struct { uint8_t data[255]; } cmd_packet_t;

class RXTask: public Task<1000>
{
public:
    RXTask(uint8_t priority, uint16_t rx_interval_ms);
    void activity();
private:
    const uint16_t rx_interval_ms; // time to wait before checking command buffer
    void readInput(); // check if input is available, returns true if successful read
    uint8_t readByte(); // read a byte from the input buffer (radio or serial)
    MsgBuffer<cmd_packet_t, 2550> cmdbuf; // buffer for input - can hold 10 commands before getting full
    StaticJsonDocument<255> curr_cmd; // storage for most recently received json

    TimerHandle_t pulseTimers[8]; // xTimers for callback (timer IDS correspond to the array index & solenoid channel)
    StaticTimer_t pulsebufs[8]; // xTimer static buffer for pulse timers
    // TimerHandle_t launchTimer; // xTimer for launch
    // StaticTimer_t launchbuf; // xTimer static buffer for launch timer

    void process_cmd(cmd_packet_t cmd); // uses the command defs from above to determine what to do with a received command

    // functions used for executing commands
    static void open_solenoid(uint8_t sol_ch); // sets solenoid state to open
    static void open_solenoids(JsonArrayConst sol_ch); // sets multiple solenoid state to open
    static void close_solenoid(uint8_t sol_ch); // sets solenoid state of given channel to closed
    static void close_solenoids(JsonArrayConst sol_ch); // sets solenoid state of multiple channels to closed
    void pulse_solenoid(uint8_t sol_ch, uint16_t pulse_dur_ms); // reads a time in milliseconds from serial, opens solenoid, sets a timer to close solenoid after specified duration
    void pulse_solenoids(JsonArrayConst sol_ch, uint16_t pulse_dur_ms);
    static void _close_solenoid(TimerHandle_t xTimer); //close the solenoid associated with the given xTimer
    static void fire_ematch(uint8_t em_ch);
    static void fire_ematches(JsonArrayConst em_ch);

    // TODO: implement the following
    // void config_radio(); // reads radio settings from serial and sends to radio settings msgbuffer
    // void abort_fuel(); // aborts fuel-side 
    // void abort_ox(); // aborts ox-side
    // void launch(); // starts launch sequence
    // static void fire_igniter(TimerHandle_t xTimer); // fires engine igniter ematches
    // static void open_main_valves(TimerHandle_t xTimer); // opens main engine valves
    // void cancel_launch(); // cancels the launch sequence
};