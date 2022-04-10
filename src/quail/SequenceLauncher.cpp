#include "SequenceLauncher.hpp"
#include "main.hpp"

#define INCLUDE_vTaskDelete 1
#define INCLUDE_vTaskSuspend 1

uint8_t SequenceLauncher::seq_priority = 0;

// SequenceLauncher::SequenceLauncher(uint8_t priority) {
//     // seq_priority = priority;

//     // num_seqs = 5;
//     // seq_t *redSeq, *abortSeq, *dischargeOxSeq, *dischargeFuelSeq, *launchSeq;

//     // redSeq = new seq_t(new Sequence(sys.slate.sequence.redline, &redLine), 0, {});
//     // abortSeq = new seq_t(new Sequence(sys.slate.sequence.abort, &abort), 1, new seq_t*[1]{
//     //     launchSeq
//     // });
//     // dischargeOxSeq = new seq_t(new Sequence(sys.slate.sequence.dischargeOx, &dischargeOx), 1, new seq_t*[1]{
//     //     launchSeq
//     // });
//     // dischargeFuelSeq = new seq_t(new Sequence(sys.slate.sequence.dischargeFuel, &dischargeFuel), 1, new seq_t*[1]{
//     //     launchSeq
//     // });
//     // launchSeq = new seq_t(new Sequence(sys.slate.sequence.launch, &launch), 3, new seq_t*[3]{
//     //     abortSeq,
//     //     dischargeOxSeq,
//     //     dischargeFuelSeq
//     // });

//     // seqs = new seq_t*[num_seqs]{redSeq, abortSeq, dischargeOxSeq, dischargeFuelSeq, launchSeq};
// }

// bool SequenceLauncher::startSeq(std::string name, bool update) {
//     seq_t* seq;
//     if (!getSequence(name, seq)) { return false; }
//     for (uint8_t i = 0; i < seq->num_deps; i++) {
//         pauseSeq(seq->task->state.id);
//     }
//     seq->task->state << RUNNING;
//     seq->task->checkWait();
//     return true;
// }

// bool SequenceLauncher::pauseSeq(std::string name, bool update) {
//     seq_t* seq;
//     if (!getSequence(name, seq)) { return false; }
//     seq->task->state << SUSPEND;
//     seq->task->checkWait();
//     return true;
// }

// bool SequenceLauncher::stopSeq(std::string name, bool update) {
//     seq_t* seq;
//     if (!getSequence(name, seq)) { return false; }
//     seq->task->state << DELETE;
//     seq->task->checkWait();
//     return true;
// }

// bool SequenceLauncher::getSequence(std::string name, SequenceLauncher::seq_t*& seq) {
//     for (uint8_t i = 0; i < num_seqs; i++) {
//         if (name == seqs[i]->task->state.id) {
//             seq = seqs[i];
//             return true;
//         }
//     }
//     return false;
// }

// // THESE ARE ALL FAKE. I DONT KNOW SHIT. PLEASE HELP

// void SequenceLauncher::redLine() {
//     sys.slate.sequence.redline << SUSPEND;
// }

// void SequenceLauncher::abort() {
//     // sys.tasks.seqlauncher.startSeq("DISCHARGEOX");
//     // sys.tasks.seqlauncher.startSeq("DISCHARGEFUEL");
//     sys.slate.sequence.abort << SUSPEND;
// }

// void SequenceLauncher::dischargeOx() {
//     sys.tasks.valvetask.closeSolenoid(5); // close ox fill
//     sys.tasks.valvetask.pulseSolenoid(6, 5000); // open ox vent
//     sys.slate.sequence.dischargeOx << SUSPEND;
// }

// void SequenceLauncher::dischargeFuel() {
//     sys.tasks.valvetask.closeSolenoid(3); // close ox fill
//     sys.tasks.valvetask.pulseSolenoid(7, 5000); // pulse fuel sump
//     sys.slate.sequence.dischargeFuel << SUSPEND;
// }

// void SequenceLauncher::launch() {
//     for (uint8_t i = 0; i < NUM_SOLENOIDS; i++) {
//         sys.tasks.valvetask.closeSolenoid(i);
//     }
//     sys.tasks.valvetask.pulseSolenoid(6, 5000); // bleed ox
//     sys.tasks.valvetask.pulseSolenoid(4, 5000); // bleed fuel
//     vTaskDelay(5000 / portTICK_RATE_MS);
//     sys.tasks.firetask.fireEmatch(1);
//     sys.tasks.valvetask.openSolenoid(1, false);
//     sys.tasks.valvetask.openSolenoid(2, true);
//     sys.slate.sequence.launch << SUSPEND;
// }