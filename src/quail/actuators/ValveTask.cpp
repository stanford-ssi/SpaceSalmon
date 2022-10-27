#include "ValveTask.hpp"
#include "main.hpp"

ValveTask::ValveTask(uint8_t priority) : Task(priority, "Valves"), valve1(sys.telem_slate.s1, sys.telem_slate.s1_pulse, 22) {}