#include "ValveTask.hpp"
#include "Arduino.h"
#include <cstring>

ValveTask::ValveTask(uint8_t priority, uint8_t numValves) : Task(priority, "Valves"), numValves(numValves){
    configure();
}

ValveTask::ValveTask(uint8_t priority) : Task(priority, "Valves"), numValves(MAX_SOLENOIDS){
    configure();
}

void ValveTask::configure() {
    valveManager = xEventGroupCreateStatic(&evBuf);
    valves[0] = {"S3", 2, 122};
    valves[0] = {"S4", 3, 122};
    valves[0] = {"S5", 4, 122};
    valves[0] = {"S6", 5, 122};
    valves[0] = {"S7", 6, 122};
    valves[0] = {"S8", 7, 122};
    for (uint8_t i = 0; i < numValves; i++) {
        pinMode(SOLENOID_START + valves[i].num, OUTPUT);
    }
}


void ValveTask::activity() {
    while(true) {
        uint32_t flags = xEventGroupWaitBits(valveManager, 0xff, true, false, NEVER);
        for(uint8_t i = 0; i < numValves; i++) {
            if(flags & (1UL << valves[i].num)) { 
                analogWrite(SOLENOID_START + valves[i].num, valves[i].pwm);
            }
        }
    }
}

void ValveTask::openValve(const uint8_t valve) {
    xEventGroupSetBits(valveManager, 1UL<<valve);
}

void ValveTask::openValve(const char* name) {
    for (uint8_t i = 0; i < numValves; i++) {
        if (strcmp(name, valves[i].name)) {
            openValve(valves[i].num);
        }
    }
}

bool ValveTask::checkValve(const uint8_t valve) {
    uint32_t flags = xEventGroupGetBits(valveManager);
    return flags & (1 << valve);
}

bool ValveTask::checkValve(const char* name) {
    for (uint8_t i = 0; i < numValves; i++) {
        if (strcmp(name, valves[i].name)) {
            return checkValve(valves[i].num);
        }
    }
    return false;
}