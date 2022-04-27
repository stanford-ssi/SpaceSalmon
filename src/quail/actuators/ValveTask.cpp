#include "ValveTask.hpp"
#include "Arduino.h"
#include "main.hpp"

TimerHandle_t ValveTask::pulseTimers[NUM_SOLENOIDS];
StaticTimer_t ValveTask::pulseBufs[NUM_SOLENOIDS];

ValveTask::ValveTask(uint8_t priority, uint8_t valve_pin_start) : 
Task(priority, "Valves"), valve_pin_start(valve_pin_start), slate(sys.slate.valves){
    valveManager = xEventGroupCreateStatic(&evBuf);
    for (uint8_t i = 0; i < NUM_SOLENOIDS; i++) {
        pinMode(valve_pin_start + i, OUTPUT);
    }

    for(uint8_t i = 0; i < NUM_SOLENOIDS; i++) {
        pulseTimers[i] = xTimerCreateStatic(slate[i].id.c_str(), // timer identifier
                                            slate[i].pulse(), // default pulse time (always modified before calling)
                                            pdFALSE, // timer does NOT auto-reload
                                            (void *) (long)i, // timer ID, used to know which solenoid to close on callback
                                            _callback, // callback to close
                                            &(pulseBufs[i])); // static buffer
    }
}

void ValveTask::activity() {
    while(true) {
        xEventGroupWaitBits(valveManager, UPDATE_VALVES, true, false, NEVER);
        for(uint8_t i = 0; i < NUM_SOLENOIDS; i++) {
            if(slate[i].state() == slate[i].normal()) { // if valve is in the state in which it should be powered
                if(digitalPinHasPWM(valve_pin_start + i))
                    analogWrite(valve_pin_start + i, slate[i].pwm());
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

bool ValveTask::updatePulse(uint8_t ch, uint16_t pulse_dur) {
    if (ch < 0 || ch >= NUM_SOLENOIDS) return false;
    slate[ch].pulse << pulse_dur;
    xTimerChangePeriod(pulseTimers[ch], slate[ch].pulse(), NEVER); // set new pulse period
}

bool ValveTask::pulseSolenoid(uint8_t ch){
    bool ret = openSolenoid(ch); // open solenoid
    xTimerStart(pulseTimers[ch], NEVER); // start the timer to close this solenoid
    return ret;
};

bool ValveTask::pulseSolenoid(uint8_t ch, uint16_t pulse_dur) {
    return updatePulse(ch, pulse_dur) && pulseSolenoid(ch);
}

bool ValveTask::_updateSolenoid(uint8_t ch, bool update_valves, solenoid_state_t state) {
    if(ch >= 0 && ch < NUM_SOLENOIDS){
        slate[ch].state << state;
        if (update_valves) {
            _updateValves();
        }
        return true;
    } else { return false; }
}

void ValveTask::_updateValves() {
    xEventGroupSetBits(valveManager, UPDATE_VALVES);
}

void ValveTask::_callback(TimerHandle_t xTimer){
    xTimerStop(xTimer, NEVER); // reset timer so its no longer active
    uint8_t sol_ch[4];
    sol_ch[0] = (long) pvTimerGetTimerID(xTimer);
    // sys.tasks.valvetask.closeSolenoid(sol_ch[0]); // circuitous to keep static
};