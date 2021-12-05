#include "RXTask.hpp"
#include "main.hpp"

StaticJsonDocument<1024> RXTask::usercmds;

RXTask::RXTask(uint8_t priority, uint16_t rx_interval):Task(priority, "RX"), rx_interval_ms(rx_interval){
    for(uint8_t i = 0; i < 8; i++) {
        char svname[4] = {'S','V',(char)((uint8_t)'0'+i),'\0'};
        pulseTimers[i] = xTimerCreateStatic(svname, // timer identifier
                                            100, // default pulse time (always modified before calling)
                                            pdFALSE, // timer does NOT auto-reload
                                            (void *) (long)i, // timer ID, used to know which solenoid to close on callback
                                            RXTask::_close_solenoid, // callback
                                            &(pulsebufs[i])); // static buffer

    }
    //waitTimer = xTimerCreateStatic("wait",10000,pdFALSE,(void*)0, RXTask::_close_solenoid, &waitbuf);
    //deserializeJson(RXTask::usercmds, "{\"abortOx\":{\"openSV\":[1,2]},\"launch\":[{\"closeSV\":{2,3}}, {\"waitThen\":{\"fireEM\":1, \"waitThen\":{\"openSV\":[1,2]}, \"waitTime\":4000}, \"waitTime\":10000}] }");
};

void RXTask::activity(){
    TickType_t lastSensorTime = xTaskGetTickCount();
    // every rx_interval_ms, get all commands sent since last checked and process them
    cmd_packet_t cmd;
    while(true){
        readInput(); // update cmdbuf from input, either radio or serial
        while(!cmdbuf.empty()){
            cmdbuf.receive(cmd, false); // receive raw bit data
            // convert bit data to JSON
            curr_cmd.clear();
            DeserializationError ret = deserializeJson(curr_cmd, cmd.data);
            if(ret == DeserializationError::Ok)
                process_cmd_json(curr_cmd.to<JsonObject>());
        }
        vTaskDelayUntil(&lastSensorTime, rx_interval_ms); // wait for a while to allow other tasks time to operate
    }
};

void RXTask::process_cmd_array(JsonArrayConst cmd_array){
    for(uint8_t i = 0; i < cmd_array.size(); i++){
        if(cmd_array[i].is<JsonArray>())
            process_cmd_array(cmd_array[i]);
        else
            process_cmd_json(cmd_array[i]);
    }    
}

void RXTask::process_cmd_json(JsonObjectConst cmd){ // parses a data string for quail primitive commands or user-defined commands

    if(cmd.containsKey("openSV")){
        if(cmd["openSV"].is<int>())
            open_solenoid(curr_cmd["openSV"]);
        else if(cmd["openSV"].is<JsonArray>())
            open_solenoids(cmd["openSV"]);
    }
    if(cmd.containsKey("closeSV")){
        if(cmd["closeSV"].is<int>())
            close_solenoid(cmd["closeSV"]);
        else if(cmd["closeSV"].is<JsonArray>())
            close_solenoids(cmd["closeSV"]);
    }
    if(cmd.containsKey("pulseSV") && cmd.containsKey("pulseTime")) {
        if(cmd["pulseSV"].is<int>())
            pulse_solenoid(cmd["pulseSV"], cmd["pulseTime"]);
        else if(cmd["pulseSV"].is<JsonArray>())
            pulse_solenoids(cmd["pulseSV"],  cmd["pulseTime"]);
    }
    if(cmd.containsKey("fireEM")){
        if(cmd["fireEM"].is<int>())
            fire_ematch(cmd["fireEM"]);
        else if(cmd["fireEM"].is<JsonArray>())
            fire_ematches(cmd["fireEM"]);
    }
};

void RXTask::pulse_solenoid(uint8_t sol_ch, uint16_t pulse_dur){
    xTimerChangePeriod(pulseTimers[sol_ch], pulse_dur, NEVER); // set new pulse period
    open_solenoid(sol_ch); // open solenoid
    xTimerStart(pulseTimers[sol_ch], NEVER); // start the timer to close this solenoid
};

void RXTask::pulse_solenoids(JsonArrayConst sol_ch, uint16_t pulse_dur){
    uint8_t num_ch = sol_ch.size();
    for(uint8_t i = 0; i < num_ch; i++)
        xTimerChangePeriod(pulseTimers[(uint8_t)sol_ch[i]], pulse_dur, NEVER); // set new pulse period
    open_solenoid(sol_ch); // open solenoids
    for(uint8_t i = 0; i < num_ch; i++)
        xTimerStart(pulseTimers[(uint8_t)sol_ch[i]], NEVER); // start the timer to close this solenoid
};

void RXTask::open_solenoid(uint8_t sol_ch){
    sys.statedata.setSolenoidState(sol_ch, OPEN);
};

void RXTask::open_solenoids(JsonArrayConst sol_ch){
    uint8_t num_ch = sol_ch.size();
    for(uint8_t i = 0; i < num_ch; i++)
        sys.statedata.setSolenoidState(sol_ch[i], OPEN, i==num_ch); // update all solenoid states before flagging ValveTask
};

void RXTask::close_solenoid(uint8_t sol_ch){
    sys.statedata.setSolenoidState(sol_ch, CLOSED); // update all solenoid states before flagging ValveTask
};

void RXTask::close_solenoids(JsonArrayConst sol_ch){
    uint8_t num_ch = sol_ch.size();
    for(uint8_t i = 0; i < num_ch; i++)
        sys.statedata.setSolenoidState(sol_ch[i], CLOSED, i==num_ch);
};

void RXTask::_close_solenoid(TimerHandle_t xTimer){
    sys.statedata.setSolenoidState((long) pvTimerGetTimerID(xTimer), CLOSED);
};

void RXTask::fire_ematch(uint8_t em_ch){
    sys.statedata.fireEmatch(em_ch);
};

void RXTask::fire_ematches(JsonArrayConst em_ch){
    uint8_t num_ch = em_ch.size();
    for(uint8_t i = 0; i < num_ch; i++)
        sys.statedata.fireEmatch(em_ch[i], i==num_ch);
};

// void RXTask::wait_then(JsonObjectConst cmd, uint16_t wait_time){
//     // check that wait timer isn't already in use
//     if(xTimerIsTimerActive(waitTimer) != pdFALSE)
//         return; // if timer is already active, don't interrupt it - fail silently
//     wait_cmd.clear();
//     wait_cmd = cmd; // update the wait command
//     xTimerChangePeriod(waitTimer, wait_time, NEVER); // set wait time
//     xTimerStart(waitTimer, NEVER); // start the timer to close this solenoid
// };

// void RXTask::wait_callback(TimerHandle_t xTimer){
//     process_cmd_json(wait_cmd.to<JsonObject>()); 
// };

void RXTask::sendcmd(const char* cmd){
    cmd_packet_t packet_in;
    memcpy(packet_in.data, cmd, MAX_CMD_LENGTH);
    cmdbuf.send(packet_in);
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
        Serial.readBytesUntil('\n',packet_in.data,255); // endline indicates end of a json cmd
        cmdbuf.send(packet_in);
    }
};