#include "SequenceLauncher.hpp"
#include "main.hpp"

SequenceLauncher::SequenceLauncher() {
    seq_t redSeq, abortSeq, fillOxSeq, fillFuelSeq, ventOxSeq, ventFuelSeq;

    seq_t redSeq = {"REDLINE", new Sequence(1, &redLine), 0, {}};
    seq_t abortSeq = {"ABORT", new Sequence(1, &abort), 5, new seq_t[5]{
        abortSeq,
        fillOxSeq,
        fillFuelSeq,
        ventOxSeq,
        ventFuelSeq
    }};
    seq_t fillOxSeq = {"FILLOX", new Sequence(1, &fillOx), 1, new seq_t[1]{
        ventOxSeq
    }};
    seq_t fillFuelSeq = {"FILLFUEL", new Sequence(1, &fillFuel), 1, new seq_t[1]{
        ventFuelSeq
    }};
    seq_t ventOxSeq = {"VENTOX", new Sequence(1, &ventOx), 1, new seq_t[1]{
        fillOxSeq
    }};
    seq_t ventFuelSeq = {"VENFUEL", new Sequence(1, &ventFuel), 1, new seq_t[1]{
        fillFuelSeq
    }};

    num_seqs = 6;
    seq_t seqArr[num_seqs] = {redSeq, abortSeq, fillOxSeq, fillFuelSeq, ventOxSeq, ventFuelSeq};
    seqs = seqArr;

    startSeq("REDLINE");
}

bool SequenceLauncher::startSeq(char* name) {
    seq_t* seq;
    if(!getSequence(name, seq)) return false;
    for (uint8_t i = 0; i < seq->num_deps; i++) {
        pauseSeq(seq->deps[i].name);
    }
    seq->task->resume();
    return true;
}

bool SequenceLauncher::stopSeq(char* name) {
    seq_t* seq;
    if(!getSequence(name, seq)) return false;
    seq->task->stop();
}

bool SequenceLauncher::pauseSeq(char* name) {
    seq_t* seq;
    if(!getSequence(name, seq)) return false;
    seq->task->suspend();
}

void SequenceLauncher::redLine() {
    if(sys.slate.sense.pt1 > MAWP || sys.slate.sense.pt2 > MAWP) {
        abort();
    }
}

void SequenceLauncher::abort() {
    ventOx();
    ventFuel();
}

void SequenceLauncher::fillOx() {
    while(sys.slate.sense.pt1 < EWP) {
        sys.tasks.valvetask.pulseSolenoid(1, 100);
    }
}

void SequenceLauncher::fillFuel() {
    while(sys.slate.sense.pt2 < EWP) {
        sys.tasks.valvetask.pulseSolenoid(2, 100);
    }
}

void SequenceLauncher::ventOx() {
    while(sys.slate.sense.pt1 > 0) {
        sys.tasks.valvetask.pulseSolenoid(3, 100);
    }
}

void SequenceLauncher::ventFuel() {
    while(sys.slate.sense.pt2 > 0) {
        sys.tasks.valvetask.pulseSolenoid(4, 100);
    }
}

bool SequenceLauncher::getSequence(char* name, SequenceLauncher::seq_t* seq) {
    for (uint8_t i = 0; i < num_seqs; i++) {
        if (strcmp(name, seqs[i].name)) {
            seq = &seqs[i];
            return true;
        }
    }
    return false;
}