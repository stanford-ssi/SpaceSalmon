#include "ValveTask.hpp"
#include "Arduino.h"
#include "main.hpp"

TimerHandle_t ValveTask::pulseTimers[NUM_SOLENOIDS];
StaticTimer_t ValveTask::pulseBufs[NUM_SOLENOIDS];

ValveTask::ValveTask(uint8_t priority, uint8_t valve_pin_start) : Task(priority, "Valves"), valve_pin_start(valve_pin_start){
    valveManager = xEventGroupCreateStatic(&evBuf);
    valves[0] = {"S1", MEDIUM, NORMALLY_CLOSED};
    valves[1] = {"S2", MEDIUM, NORMALLY_CLOSED};
    valves[2] = {"S3", MEDIUM, NORMALLY_CLOSED};
    valves[3] = {"S4", MEDIUM, NORMALLY_CLOSED};
    valves[4] = {"S5", MEDIUM, NORMALLY_CLOSED};
    valves[5] = {"S6", MEDIUM, NORMALLY_CLOSED};
    valves[6] = {"S7", MEDIUM, NORMALLY_CLOSED};
    valves[7] = {"S8", MEDIUM, NORMALLY_CLOSED};
    for (uint8_t i = 0; i < NUM_SOLENOIDS; i++) {
        pinMode(valve_pin_start + i, OUTPUT);
    }

    for(uint8_t i = 0; i < NUM_SOLENOIDS; i++) {
        char svname[4] = {'S','V',(char)((uint8_t)'0'+i),'\0'};
        pulseTimers[i] = xTimerCreateStatic(svname, // timer identifier
                                            100, // default pulse time (always modified before calling)
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
            if(sys.slate.solenoid[i] == valves[i].normal) { // if valve is in the state in which it should be powered
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

bool ValveTask::pulseSolenoid(uint8_t ch, uint16_t pulse_dur){
    if (ch < 0 || ch >= NUM_SOLENOIDS) return false;
    xTimerChangePeriod(pulseTimers[ch], pulse_dur, NEVER); // set new pulse period
    bool ret = openSolenoid(ch); // open solenoid
    xTimerStart(pulseTimers[ch], NEVER); // start the timer to close this solenoid
    return ret;
};

bool ValveTask::_updateSolenoid(uint8_t ch, bool update_valves, solenoid_state_t state) {
    if(ch >= 0 && ch < NUM_SOLENOIDS){
        if( state == valves[ch].normal) // if trying to power the valve
            if( !(num_valves_powered < MAX_NUM_VALVES_POWERED) )
                return true; // if already at max number, just return
            else
                num_valves_powered++; // if good to power, increase count
        else // if trying to un-power valve
            num_valves_powered--;
        sys.slate.solenoid[ch] = state;
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
    sys.tasks.valvetask.closeSolenoid(sol_ch[0]); // circuitous to keep static
};