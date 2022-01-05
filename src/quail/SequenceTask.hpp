#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "Task.hpp"
#include "event_groups.h"
#include <hal_rtos.h>
#include "MsgBuffer.hpp"
#include "SSISD.hpp"

#define MAX_TEST_FILENAME_SIZE 20
#define SEQ_STARTED 0b1
#define SEQ_PLAYING 0b10
#define SEQ_LOADED 0b100

class SequenceTask : public Task<2000>{
public:
    SequenceTask(uint8_t priority);
    void activity();

    void start_seq();
    void stop_seq();
    void pause_seq();
    void play_seq();
    void delay_seq(uint16_t ticks_to_wait);
    void load_seq(const char* seq_name);

    bool isSequenceStarted(){return playing;};
    bool isSequencePlaying(){return started;};

private:
    bool playing = false;
    bool started = false;

    static SequenceTask* glob_ptr; // global pointer for accessing event group from callback
    static void _play_seq(TimerHandle_t xTimer);
    void _load_seq();

    FIL seq_file_object; // sequence file object
    MsgBuffer<FIL, sizeof(FIL)*2> filebuf; // file object buffer
    char seqLineBuffer[1000];

    MsgBuffer<char[MAX_TEST_FILENAME_SIZE], MAX_TEST_FILENAME_SIZE*2> seqbuf; // message buffer for sending new sequence name
    
    StaticEventGroup_t evBuf;
    EventGroupHandle_t evGroup;

    static TimerHandle_t delayTimer; // xTimer for callback commands
    static StaticTimer_t delaybuf; // xTimer static buffer for waitTimer
};