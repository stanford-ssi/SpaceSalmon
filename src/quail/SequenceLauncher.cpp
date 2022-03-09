#include "SequenceLauncher.hpp"
#include "main.hpp"

#define INCLUDE_vTaskDelete 1
#define INCLUDE_vTaskSuspend 1

uint8_t SequenceLauncher::seq_priority = 0;

SequenceLauncher::SequenceLauncher(uint8_t priority) {
    seq_priority = priority;
    seq_t *redSeq, *abortSeq, *fillOxSeq, *fillFuelSeq, *ventOxSeq, *ventFuelSeq;

    redSeq = new seq_t(new Sequence(sys.slate.sequence.redline, &redLine), 0, {});
    abortSeq = new seq_t(new Sequence(sys.slate.sequence.abort, &abort), 4, new seq_t*[4]{
        fillOxSeq,
        fillFuelSeq,
        ventOxSeq,
        ventFuelSeq
    });
    fillOxSeq = new seq_t(new Sequence(sys.slate.sequence.fillox, &fillOx), 1, new seq_t*[1]{
        ventOxSeq
    });
    fillFuelSeq = new seq_t(new Sequence(sys.slate.sequence.fillfuel, &fillFuel), 1, new seq_t*[1]{
        ventFuelSeq
    });
    ventOxSeq = new seq_t(new Sequence(sys.slate.sequence.ventox, &ventOx), 1, new seq_t*[1]{
        fillOxSeq
    });
    ventFuelSeq = new seq_t(new Sequence(sys.slate.sequence.ventfuel, &ventFuel), 1, new seq_t*[1]{
        fillFuelSeq
    });

    num_seqs = 6;
    seqs = new seq_t*[num_seqs]{redSeq, abortSeq, fillOxSeq, fillFuelSeq, ventOxSeq, ventFuelSeq};
}

bool SequenceLauncher::startSeq(std::string name, bool update) {
    sys.tasks.txtask.writeUSB("Starting...");
    seq_t* seq;
    if (!getSequence(name, seq)) { return false; }
    for (uint8_t i = 0; i < seq->num_deps; i++) {
        pauseSeq(seq->task->state.id);
    }
    seq->task->state = RUNNING;
    seq->task->checkWait();
    return true;
}

bool SequenceLauncher::pauseSeq(std::string name, bool update) {
    sys.tasks.txtask.writeUSB("Suspending...");
    seq_t* seq;
    if (!getSequence(name, seq)) { return false; }
    seq->task->state = SUSPEND;
    seq->task->checkWait();
    return true;
}

bool SequenceLauncher::stopSeq(std::string name, bool update) {
    seq_t* seq;
    if (!getSequence(name, seq)) { return false; }
    seq->task->state = DELETE;
    seq->task->checkWait();
    return true;
}

void SequenceLauncher::redLine() {
    bool oxOverPressure = sys.slate.sense.pt6 > 600;
    bool fuelOverPressure = sys.slate.sense.pt5 > 600;
    bool ccOverPressure = sys.slate.sense.pt7 > 600;
    // presumable also some temperature stuff
    if (oxOverPressure || fuelOverPressure || ccOverPressure) {
        abort();
    }
}

bool SequenceLauncher::getSequence(std::string name, SequenceLauncher::seq_t*& seq) {
    for (uint8_t i = 0; i < num_seqs; i++) {
        if (name == seqs[i]->task->state.id) {
            seq = seqs[i];
            return true;
        }
    }
    return false;
}

// THESE ARE ALL FAKE. I DONT KNOW SHIT. PLEASE HELP
void SequenceLauncher::abort() {
    /**
     * 1. Close fill valves
     * 2. Close main valves
     * 3. Open bleed valves
     * 4. Open discharge valves
     * 5. Open vent vales
     */
    sys.tasks.valvetask.closeSolenoid(5); // close ox fill
    sys.tasks.valvetask.closeSolenoid(3); // close fuel fill
    sys.tasks.valvetask.closeSolenoid(2); // close ox main
    sys.tasks.valvetask.closeSolenoid(1); // close fuel main
    sys.tasks.valvetask.openSolenoid(6); // open ox bleed
    // sys.tasks.valvetask.openSolenoid(7); // open fuel bleed
    // sys.tasks.valvetask.openSolenoid(8); // open ox vent
}

void SequenceLauncher::fillOx() {
    sys.tasks.txtask.writeUSB("Filling ox");
    sys.tasks.valvetask.openSolenoid(5); // open ox fill
    sys.tasks.valvetask.openSolenoid(4);
    // vTaskDelay(5000 / portTICK_RATE_MS);
    // sys.tasks.valvetask.closeSolenoid(6);
    // sys.tasks.valvetask.pulseSolenoid(6, 1000); // pulse ox vent (pull in liquid)
}

void SequenceLauncher::fillFuel() {
    sys.tasks.txtask.writeUSB("Filling fuel");
    sys.tasks.valvetask.openSolenoid(3);
}

void SequenceLauncher::ventOx() {
    sys.tasks.valvetask.openSolenoid(6);
}

void SequenceLauncher::ventFuel() {
    sys.tasks.valvetask.openSolenoid(7);
}