#include "RXTask.hpp"
#include "main.hpp"

RXTask::RXTask(uint8_t priority, uint16_t rx_interval):Task(priority, "RX"), rx_interval_ms(rx_interval){
    for(uint8_t i = 0; i < 8; i++) {
        char svname[4] = {'S','V',(char)((uint8_t)'0'+i),'\0'};
        pulseTimers[i] = xTimerCreateStatic(svname, // timer identifier
                                            100, // default pulse time (always modified before calling)
                                            pdFALSE, // timer does NOT auto-reload
                                            (void *) (long)i, // timer ID, used to know which solenoid to close on callback
                                            RXTask::close_solenoid, // callback
                                            &(pulsebufs[i])); // static buffer

    }
    // launchTimer = xTimerCreateStatic("launch",10000,pdFALSE,(void*)0, RXTask::fire_igniter, &launchbuf);
};

void RXTask::activity(){
    Serial.setTimeout(rx_interval_ms/2); // set serial timeout
    TickType_t lastSensorTime = xTaskGetTickCount();
    // every rx_interval_ms, get all commands sent since last checked and process them
    uint8_t cmd;
    while(true){
        readInput(); // update cmdbuf from input, either radio or serial
        
        while(!cmdbuf.empty()){

            cmdbuf.receive(cmd, false);
            if(cmd != CMD_ENDLINE) {// if you received a non-endline byte
                Serial.println(cmd);
                process_cmd(cmd);
            }
            while(cmd != CMD_ENDLINE && !cmdbuf.empty()){ // after processing a command, clear until an endline
                cmdbuf.receive(cmd, false);
            }
        }
        vTaskDelayUntil(&lastSensorTime, rx_interval_ms); // wait for a while to allow other tasks time to operate
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
    uint8_t byte_in;
    if(cmdbuf.receive(byte_in, false) && byte_in > 0){  // if receieved a valid pulse time
        uint16_t pulse_dur = 10*byte_in; // pulse time is 10* received value in ms
        xTimerChangePeriod(pulseTimers[sol_ch], pulse_dur, NEVER); // set new pulse period
        open_solenoid(sol_ch); // open solenoid
        xTimerStart(pulseTimers[sol_ch], NEVER); // start the timer to close this solenoid
    }
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

void RXTask::readInput(){
    #ifdef RADIO_TXRX
        while(sys.tasks.radiotask.packetAvailable()){
            packet_t packet_in;
            sys.tasks.radiotask.waitForPacket(packet_in); 
            if(packet_in.data[packet_in.length-1] == CMD_ENDLINE) // ensure cmd is terminated (multi-cmd packets allowed, but final one must be terminated)
                for(uint8_t i = 0; i < packet_in.length; i++)
                    cmdbuf.send(packet_in.data[i]); // add data byte to the cmd buffer
        }
    #else
        uint8_t bytes_in[MAX_CMD_LENGTH+1];
        bool missed_endline = false;
        while(Serial.available()){
            if(Serial.readBytesUntil(0,bytes_in,MAX_CMD_LENGTH+1) < (MAX_CMD_LENGTH+1)) { // try to read a command terminated by null byte
                if(missed_endline) { // if you missed an endline on your last read attempt, discard commands until a new endline
                    for(uint8_t i = 0; i < MAX_CMD_LENGTH; i++) {                 
                        cmdbuf.send(bytes_in[i]); // add byte to the cmd buffer
                        if(bytes_in[i] != CMD_ENDLINE)
                            break; // if reached an endline, finished processing command!
                    }
                }
                missed_endline = false; // if you got here, you received an endline-terminated cmd
            }
            else {
                missed_endline = true; // if you got here, you timed-out or read too much waiting for an endline
            }
        }
    #endif
};