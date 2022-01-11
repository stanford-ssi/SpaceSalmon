#include "ValveTask.hpp"
#include "Arduino.h"
#include "main.hpp"

ValveTask::ValveTask(uint8_t priority, uint8_t valve_pin_start) : Task(priority, "Valves"), valve_pin_start(valve_pin_start){
    valveManager = xEventGroupCreateStatic(&evBuf);
    valves[0] = {"S1", REAL_SMALL, NORMALLY_CLOSED};
    valves[1] = {"S2", P_SMALL, NORMALLY_CLOSED};
    valves[2] = {"S3", SMALL, NORMALLY_CLOSED};
    valves[3] = {"S4", REAL_SMALL, NORMALLY_CLOSED};
    valves[4] = {"S5", REAL_SMALL, NORMALLY_CLOSED};
    valves[5] = {"S6", P_SMALL, NORMALLY_CLOSED};
    valves[6] = {"S7", ABORT, NORMALLY_CLOSED}; // Warning: These can't do PWM
    valves[7] = {"S8", ABORT, NORMALLY_CLOSED}; // Warning: These can't do PWM
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
                if(digitalPinHasPWM(valve_pin_start + i))
                    analogWrite(valve_pin_start + i, valves[i].pwm);
                else
                    digitalWrite(valve_pin_start + i, HIGH);
            }
            else{
                if(digitalPinHasPWM(valve_pin_start + i))
                    analogWrite(valve_pin_start + i, 0);
                else
                    digitalWrite(valve_pin_start + i, LOW);
            }
        }
    }
}

void ValveTask::updateValves() {
    xEventGroupSetBits(valveManager, VALVES_UPDATED);
}
