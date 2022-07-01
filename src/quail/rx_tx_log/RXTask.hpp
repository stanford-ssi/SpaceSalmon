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
#include "../config.h"

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

    TimerHandle_t commTimer;
    StaticTimer_t commTimerBuf;
    // TODO: implement the following
    // void config_radio(); // reads radio settings from serial and sends to radio settings msgbuffer
};