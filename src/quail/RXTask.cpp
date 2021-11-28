#include "RXTask.hpp"
#include "main.hpp"

RXTask::RXTask(uint8_t priority, uint8_t rx_interval):Task(priority, "RX"), rx_interval_ms(rx_interval){
    for(uint8_t i = 0; i < 8; i++) {
        char svname[4] = {'S','V','0'+i,'\0'};
        pulseTimers[i] = xTimerCreateStatic(svname, // timer identifier
                                            100, // default pulse time (always modified before calling)
                                            pdFALSE, // timer does NOT auto-reload
                                            (void *) i, // timer ID, used to know which solenoid to close on callback
                                            RXTask::close_solenoid, // callback
                                            &(pulsebufs[i])); // static buffer

    }
    // launchTimer = xTimerCreateStatic("launch",10000,pdFALSE,(void*)0, RXTask::fire_igniter, &launchbuf);
};

void RXTask::activity(){
    Serial.setTimeout(rx_interval_ms/2); // set serial timeout
    TickType_t lastSensorTime = xTaskGetTickCount();
    // every rx_interval_ms, get all commands sent since last checked and process them
    while(true){
        char buf[3];
        while(Serial.available()){ // typically we won't send such frequent commands that this becomes overly blocking
            Serial.readBytes(buf, 1); // read a single byte
            uint8_t cmd_rcvd = buf[0]; // convert that byte to uint
            if(cmd_rcvd > 19) {// new lines characters or other return characters are ignored
                process_cmd(cmd_rcvd); // if a valid command, pass to process_cmd  
                if(Serial.available())
                    Serial.readBytesUntil('\n', buf, 2); // all commands end in newline char, clear to next command
            }
        }
        vTaskDelayUntil(&lastSensorTime, rx_interval_ms); // wait for a bit
    }
};

void RXTask::process_cmd(uint8_t cmd){
    switch(cmd/10){
        case OPEN_SV:
            open_solenoid(cmd%10); break;
        case CLOSE_SV:
            close_solenoid(cmd%10); break;
        case PULSE_SV:
            pulse_solenoid(cmd%10); break;
        case FIRE_EM:
            fire_ematch(cmd%10); break;
        default:
            // TODO: TX an error to show that command was not processed
            break;
    }
};

void RXTask::pulse_solenoid(uint8_t sol_ch){
    uint16_t pulse_dur;
    if(Serial.available()){ // if you got here, you should have a pulse time character following
        char q;
        Serial.readBytes(&q,1); // read a single byte
        pulse_dur = 10* ((uint8_t) q); // get pulse time in 10s of milliseconds
    } 
    else
        return; // if you don't specify a pulse time, don't do anything
    // TODO: send error indication that pulse attempt failed
    xTimerChangePeriod(pulseTimers[sol_ch], pulse_dur, NEVER); // set new pulse period
    open_solenoid(sol_ch); // open solenoid
    xTimerStart(pulseTimers[sol_ch], NEVER); // start the timer to close this solenoid
};

void RXTask::open_solenoid(uint8_t sol_ch){
    sys.statedata.setSolenoidState(sol_ch, OPEN);
};

void RXTask::close_solenoid(uint8_t sol_ch){
    sys.statedata.setSolenoidState(sol_ch, CLOSED);
};

void RXTask::close_solenoid(TimerHandle_t xTimer){
    sys.statedata.setSolenoidState((long) pvTimerGetTimerID(xTimer), CLOSED);
};

void RXTask::fire_ematch(uint8_t em_ch){
    sys.statedata.fireEmatch(em_ch);
};