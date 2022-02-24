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

#define MAX_CMD_LENGTH 255 // maximum length of a command, in bytes (based on radio command max size)
#define CMD_ENDLINE 0x0A // endline indicator for a command - null char

typedef struct{
    StaticJsonDocument<MAX_CMD_LENGTH> doc;
} cmd_t;

class RXTask: public Task<3000>
{
public:
    RXTask(uint8_t priority, uint16_t rx_interval_ms);
    void activity();
    void sendcmd(const char* cmd); // manually write command to the cmd buffer 
    void sendcmdJSON(StaticJsonDocument<MAX_CMD_LENGTH> cmdJSON); // manually write JSON cmd to the cmd buffer

private:
    const uint16_t rx_interval_ms; // time to wait before checking command buffer
    void readInput(); // check if input is available, returns true if successful read
    MsgBuffer<cmd_t, sizeof(cmd_t)*10> cmdbuf; // buffer for input - can hold 10 commands before getting full
    static StaticJsonDocument<MAX_CMD_LENGTH> curr_cmd;
    static StaticJsonDocument<MAX_CMD_LENGTH> wait_cmd; // array of wait commands (size sets max number of )
    static StaticJsonDocument<MAX_CMD_LENGTH*10> usercmds; // storage for usercmds, cmd strings defined by the user

    static TimerHandle_t pulseTimers[8]; // xTimers for callback (timer IDS correspond to the array index & solenoid channel)
    static StaticTimer_t pulsebufs[8]; // xTimer static buffer for pulse timers
    static TimerHandle_t waitTimer; // xTimer for waitThen commands
    static StaticTimer_t waitbuf; // xTimer static buffer for waitTimer

    static void process_cmd_json(JsonObjectConst cmd); // uses the command defs from above to determine what to do with a received command
    static void process_cmd_array(JsonArrayConst cmd_arr);

    // functions used for executing commands
    static void open_solenoid(uint8_t sol_ch, bool update_valves = true); // sets solenoid state to open
    static void open_solenoids(JsonArrayConst sol_ch); // sets multiple solenoid state to open
    static void close_solenoid(uint8_t sol_ch, bool update_valves = true); // sets solenoid state of given channel to closed
    static void close_solenoids(JsonArrayConst sol_ch); // sets solenoid state of multiple channels to closed
    static void pulse_solenoid(uint8_t sol_ch, uint16_t pulse_dur_ms); // reads a time in milliseconds from serial, opens solenoid, sets a timer to close solenoid after specified duration
    static void pulse_solenoids(JsonArrayConst sol_ch, uint16_t pulse_dur_ms);
    static void _close_solenoid(TimerHandle_t xTimer); //close the solenoid associated with the given xTimer
    static void fire_ematch(uint8_t em_ch, bool update_em = true); //fire one ematch
    static void fire_ematches(JsonArrayConst em_ch); // fire multiple ematches
    static void wait_then(JsonObjectConst cmd, uint16_t wait_time_ms); // executes cmd after wait_time(ms)
    static void wait_callback(TimerHandle_t xTimer); // the callback function
    static void start_seq(const char* seq_name);
    static void stop_seq(const char* seq_name);
    static void pause_seq(const char* seq_name);
    static void play_seq(const char* seq_name);
    static void load_seq(const char* seq_name);
    static void delay_seq(uint16_t ticks_to_wait);

    // TODO: implement the following
    // void config_radio(); // reads radio settings from serial and sends to radio settings msgbuffer
};