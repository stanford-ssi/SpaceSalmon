#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "Task.hpp"
#include "event_groups.h"
#include <hal_rtos.h>

#define MAX_TEST_FILENAME_SIZE

class SequenceTask : public Task<2000>{
public:
    SequenceTask(uint8_t priority):Task(priority, "Sequence"){};
    void activity();

    static void start_seq();
    static void stop_seq();
    static void pause_seq();
    static void play_seq();
    static void load_seq(const char* seq_name);
};