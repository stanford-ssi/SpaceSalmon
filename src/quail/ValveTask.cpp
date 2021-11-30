#include "ValveTask.hpp"
#include "Arduino.h"
<<<<<<< HEAD
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
=======
#include "main.hpp"

ValveTask::ValveTask(uint8_t priority, uint8_t valve_pin_start) : Task(priority, "Valves"), valve_pin_start(valve_pin_start){
    valveManager = xEventGroupCreateStatic(&evBuf);
    valves[0] = {"S1", 100, NORMALLY_CLOSED};
    valves[1] = {"S2", 100, NORMALLY_CLOSED};
    valves[2] = {"S3", 100, NORMALLY_CLOSED};
    valves[3] = {"S4", 100, NORMALLY_CLOSED};
    valves[4] = {"S5", 100, NORMALLY_CLOSED};
    valves[5] = {"S6", 100, NORMALLY_CLOSED};
    valves[6] = {"S7", 0, NORMALLY_CLOSED};
    valves[7] = {"S8", 0, NORMALLY_CLOSED};
    for (uint8_t i = 0; i < NUM_SOLENOIDS; i++) {
        pinMode(valve_pin_start + i, OUTPUT);
    }
}

void ValveTask::activity() {
    while(true) {
        xEventGroupWaitBits(valveManager, VALVES_UPDATED, true, false, NEVER);
        uint8_t valvestate = sys.statedata.getSolenoidState();
        for(uint8_t i = 0; i < NUM_SOLENOIDS; i++) {
            if((valvestate>>i & 0b1) == valves[i].normal) { // if valve is in the state in which it should be powered
                if(valves[i].pwm)
                    analogWrite(valve_pin_start + i, valves[i].pwm);
                else
                    digitalWrite(valve_pin_start + i, HIGH);
            }
            else
                digitalWrite(valve_pin_start + i, HIGH);
>>>>>>> origin/quail-dev-max
        }
    }
}

<<<<<<< HEAD
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
=======
void ValveTask::updateValves() {
    xEventGroupSetBits(valveManager, VALVES_UPDATED);
}
>>>>>>> origin/quail-dev-max
