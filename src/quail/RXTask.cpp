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
    TickType_t lastSensorTime = xTaskGetTickCount();
    // every rx_interval_ms, get all commands sent since last checked and process them
    cmd_packet_t cmd;
    while(true){
        readInput(); // update cmdbuf from input, either radio or serial
        while(!cmdbuf.empty()){
            cmdbuf.receive(cmd, false);
            process_cmd(cmd);
        }
        vTaskDelayUntil(&lastSensorTime, rx_interval_ms); // wait for a while to allow other tasks time to operate
    }
};

void RXTask::process_cmd(cmd_packet_t cmd){
    curr_cmd.clear();
    deserializeJson(curr_cmd, cmd.data); // turn string data into a json
    if(curr_cmd.containsKey("openSV"))
        open_solenoid(curr_cmd["openSV"]);
    if(curr_cmd.containsKey("closeSV"))
        close_solenoid((uint8_t) curr_cmd["closeSV"]);
    if(curr_cmd.containsKey("pulseSV") && curr_cmd.containsKey("pulseTime"))
        pulse_solenoid(curr_cmd["pulseSV"], curr_cmd["pulse"]);
    if(curr_cmd.containsKey("fireEM"))
        fire_ematch(curr_cmd["fireEM"]);
};

void RXTask::pulse_solenoid(uint8_t sol_ch, uint16_t pulse_dur){
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

void RXTask::readInput(){
    #ifdef RADIO_TXRX
        while(sys.tasks.radiotask.packetAvailable()){
            packet_t packet_in;
            sys.tasks.radiotask.waitForPacket(packet_in); 
            cmdbuf.send(packet_in.data); // add data to the cmd buffer
        }
    #endif
    while(Serial.available()){
        cmd_packet_t packet_in;
        Serial.readBytesUntil('\n',packet_in.data,255);
        cmdbuf.send(packet_in);
    }
};