#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "Task.hpp"
#include <string>
#include "event_groups.h"

#define SEQ_PRIORITY 8
#define UPDATE_SEQS 0b01

class SequenceLauncher : Task<2000> {
    public:
        SequenceLauncher(uint8_t priority);

        void activity();

        bool startSeq(std::string name, bool update = true);
        void startSeq(uint8_t i, bool update = true);

        bool stopSeq(std::string name, bool update = true);
        void stopSeq(uint8_t i, bool update = true);

        bool pauseSeq(std::string name, bool update = true);
        void pauseSeq(uint8_t i, bool update = true);

    private:
        StaticEventGroup_t evBuf;
        EventGroupHandle_t seqManager;
        void _updateSeqs();

        static void redLine();
        static void abort();
        static void fillOx();
        static void fillFuel();
        static void ventOx();
        static void ventFuel();

        class Sequence : public Task<2000> {
            public:
                Sequence(void (*action)()) : Task(SEQ_PRIORITY, "Sequence"){
                    action = action;
                }
                    
            private:
                void activity() {action();}
                // void activity() { while(true) {action();}}
                void (*action)();
        };

        struct seq_t{
            std::string name;
            Sequence* task;
            uint8_t num_deps;
            seq_t** deps;

            seq_t() : task({}), deps({}) {}
            seq_t(std::string name, Sequence* task, uint8_t num_deps, seq_t** deps) : name(name), task(task), num_deps(num_deps), deps(deps) {} 
        } typedef seq_t;

        seq_t** seqs;
        uint8_t num_seqs;

        bool getSequence(std::string name, seq_t*& seq);
        bool getSlateKey(std::string name, uint8_t* j);
};