#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "Task.hpp"
#include <map>

class SequenceLauncher {
    public:
        SequenceLauncher();

        bool startSeq(char* name);
        bool stopSeq(char* name);
        bool pauseSeq(char* name);

    private:
        static void redLine();
        static void abort();
        static void fillOx();
        static void fillFuel();
        static void ventOx();
        static void ventFuel();

        class Sequence : public Task<2000> {
            public:
                Sequence(uint8_t priority, void (*action)()) : Task(priority, "Sequence"){
                    action = action;
                }
            
            private:
                void activity() { while(true) {action();}}
                void (*action)();
        };

        struct seq_t{
            char* name;
            Sequence* task;
            uint8_t num_deps;
            seq_t* deps;

            seq_t() : task({}), deps({}) {}
            seq_t(char* name, Sequence* task, uint8_t num_deps, seq_t* deps) : name(name), task(task), num_deps(num_deps), deps(deps) {} 
        } typedef seq_t;

        seq_t* seqs;
        uint8_t num_seqs;

        bool getSequence(char* name, seq_t* seq);
};