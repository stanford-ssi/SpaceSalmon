#include "SequenceLauncher.hpp"
#include "main.hpp"

#define INCLUDE_vTaskDelete 1
#define INCLUDE_vTaskSuspend 1

SequenceLauncher::SequenceLauncher(uint8_t priority) : Task(priority, "Sequence Launcher"){
    seqManager = xEventGroupCreateStatic(&evBuf);
    
    seq_t *redSeq, *abortSeq, *fillOxSeq, *fillFuelSeq, *ventOxSeq, *ventFuelSeq;

    redSeq = new seq_t("REDLINE", new Sequence(&redLine), 0, {});
    abortSeq = new seq_t("ABORT", new Sequence(&abort), 4, new seq_t*[4]{
        fillOxSeq,
        fillFuelSeq,
        ventOxSeq,
        ventFuelSeq
    });
    fillOxSeq = new seq_t("FILLOX", new Sequence(&fillOx), 1, new seq_t*[1]{
        ventOxSeq
    });
    fillFuelSeq = new seq_t("FILLFUEL", new Sequence(&fillFuel), 1, new seq_t*[1]{
        ventFuelSeq
    });
    ventOxSeq = new seq_t("VENTOX", new Sequence(&ventOx), 1, new seq_t*[1]{
        fillOxSeq
    });
    ventFuelSeq = new seq_t("VENTFUEL", new Sequence(&ventFuel), 1, new seq_t*[1]{
        fillFuelSeq
    });

    num_seqs = 6;
    seqs = new seq_t*[num_seqs]{redSeq, abortSeq, fillOxSeq, fillFuelSeq, ventOxSeq, ventFuelSeq};

    for (uint8_t i = 0; i < num_seqs; i++) {
        seqs[i]->task->suspend();
    }
}

void SequenceLauncher::activity() {
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    startSeq("ABORT");
    while(true) {
        xEventGroupWaitBits(seqManager, UPDATE_SEQS, true, false, NEVER);
        for (uint8_t i = 0; i < num_seqs; i++) {
            seq_t* seq;
            if(!getSequence(sys.slate.sequence[i].id, seq)) continue;
            if(sys.slate.sequence[i]) {
                for (uint8_t j = 0; j < seq->num_deps; j++) {
                    // seq->deps[j]->task->suspend();
                }
                // seq->task->resume();
            } else {
                // seq->task->suspend();
            }
        }
    }
}

bool SequenceLauncher::startSeq(std::string name, bool update) {
    uint8_t i;
    if (getSlateKey(name, &i)) {
        startSeq(i, update);
        return true;
    } else {
        return false;
    }
}

void SequenceLauncher::startSeq(uint8_t i, bool update) {
    sys.slate.sequence[i] = true;
    if (update) {
        _updateSeqs();
    }
}

bool SequenceLauncher::stopSeq(std::string name, bool update) {
    return pauseSeq(name, update);
}

void SequenceLauncher::stopSeq(uint8_t i, bool update) {
    pauseSeq(i, update);
}

bool SequenceLauncher::pauseSeq(std::string name, bool update) {
    uint8_t i;
    if (getSlateKey(name, &i)) {
        stopSeq(i, update);
        return true;
    } else {
        return false;
    }
}

void SequenceLauncher::pauseSeq(uint8_t i, bool update) {
    sys.slate.sequence[i] = false;
    if (update) {
        _updateSeqs();
    }
}

void SequenceLauncher::redLine() {
    sys.tasks.valvetask.openSolenoid(1);
}

void SequenceLauncher::abort() {
    sys.tasks.valvetask.openSolenoid(1);
}

void SequenceLauncher::fillOx() {
    sys.tasks.valvetask.openSolenoid(1);
}

void SequenceLauncher::fillFuel() {
    sys.tasks.valvetask.openSolenoid(1);
}

void SequenceLauncher::ventOx() {
    sys.tasks.valvetask.openSolenoid(1);

}

void SequenceLauncher::ventFuel() {
    sys.tasks.valvetask.openSolenoid(1);
}

bool SequenceLauncher::getSequence(std::string name, SequenceLauncher::seq_t*& seq) {
    for (uint8_t i = 0; i < num_seqs; i++) {
        Serial.println(name.c_str());
        Serial.println(seqs[i]->name.c_str());
        if (name == seqs[i]->name) {
            seq = seqs[i];
            return true;
        }
    }
    return false;
}

bool SequenceLauncher::getSlateKey(std::string name, uint8_t* j) {
    for (uint8_t i = 0; i < num_seqs; i++) {
        if (name == sys.slate.sequence[i].id) {
            *j = i;
            return true;
        }
    }
    return false;
}

void SequenceLauncher::_updateSeqs() {
    xEventGroupSetBits(seqManager, UPDATE_SEQS);
}