#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "Task.hpp"
#include <string>
#include "event_groups.h"
#include "SlateKey.hpp"
#include "config.h"
#include "slate/abstractions.hpp"

class SequenceLauncher {
    public:
        SequenceLauncher(uint8_t priority);

        bool startSeq(std::string name, bool update = true);
        bool stopSeq(std::string name, bool update = true);
        bool pauseSeq(std::string name, bool update = true);

    private:
        static uint8_t seq_priority;

        static void redLine();
        static void abort();
        static void dischargeOx();
        static void dischargeFuel();
        static void launch();

        class Sequence : public Task<2000> {
            public:
                Sequence(EndPoint<SEQUENCE_STATE> &state, void (*action)()) : Task(seq_priority, state.id.c_str()), state(state), action(action) {
                    seqManager = xEventGroupCreateStatic(&evBuf);
                }                   

                void activity() {
                    while (true) {
                        switch(state()){
                            case RUNNING:                                
                                action();
                                break;
                            case SUSPEND:
                                xEventGroupWaitBits(seqManager, RUNNING, true, false, NEVER);
                                break;
                            case DELETE:
                                goto finish; // when break just isn't enough
                        }
                    }
                    finish: ;
                }

            private:
                EndPoint<SEQUENCE_STATE>& state;

                StaticEventGroup_t evBuf;
                EventGroupHandle_t seqManager;

                void checkWait() { xEventGroupSetBits(seqManager, UPDATE_SEQS); }
                void (*action)();
                
                friend SequenceLauncher;
        };

        struct seq_t{
            Sequence* task;
            uint8_t num_deps;
            seq_t** deps;

            seq_t() : task({}), deps({}) {}
            seq_t(Sequence* task, uint8_t num_deps, seq_t** deps) : task(task), num_deps(num_deps), deps(deps) {} 
        } typedef seq_t;

        seq_t** seqs;
        uint8_t num_seqs;

        bool getSequence(std::string name, seq_t*& seq);
};