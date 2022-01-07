#include "RXTask.hpp"
#include "main.hpp"

StaticJsonDocument<MAX_CMD_LENGTH*10> RXTask::usercmds;
StaticJsonDocument<MAX_CMD_LENGTH> RXTask::wait_cmd;
StaticJsonDocument<MAX_CMD_LENGTH> RXTask::curr_cmd;
TimerHandle_t RXTask::pulseTimers[8]; // xTimers for callback (timer IDS correspond to the array index & solenoid channel)
StaticTimer_t RXTask::pulsebufs[8]; // xTimer static buffer for pulse timers
TimerHandle_t RXTask::waitTimer; // xTimer for waitThen commands
StaticTimer_t RXTask::waitbuf; // xTimer static buffer for waitTimer


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
    waitTimer = xTimerCreateStatic("wait",10000,pdFALSE,(void*)0, RXTask::wait_callback, &waitbuf);
    deserializeJson(RXTask::usercmds, "{\"abortOx\":{\"closeSV\":[0,1,2,3,4,5]},\"launch\":[{\"closeSV\":{2,3}}, {\"waitThen\":{\"fireEM\":1, \"waitThen\":{\"openSV\":[1,2]}, \"waitTime\":4000}, \"waitTime\":10000}] }");
};

void RXTask::activity(){
    TickType_t lastSensorTime = xTaskGetTickCount();
    // every rx_interval_ms, get all commands sent since last checked and process them
    while(true){
        readInput(); // update cmdbuf from input, either radio or serial
        while(!cmdbuf.empty()){
            // curr_cmd.clear(); // clear command contents
            cmd_t buf;
            cmdbuf.receive(buf, false);
            curr_cmd.set(buf.doc); // copy data over
            process_cmd_json(curr_cmd.as<JsonObject>()); // pass the object pointer
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
        if(cmd["openSV"].is<unsigned int>()){
            open_solenoid(cmd["openSV"]);
        }
        else if(cmd["openSV"].is<JsonArray>())
            open_solenoids(cmd["openSV"]);
    }
    if(cmd.containsKey("closeSV")){
        if(cmd["closeSV"].is<unsigned int>())
            close_solenoid(cmd["closeSV"]);
        else if(cmd["closeSV"].is<JsonArray>())
            close_solenoids(cmd["closeSV"]);
    }
    if(cmd.containsKey("pulseSV") && cmd.containsKey("pulseTime")) {
        if(cmd["pulseSV"].is<unsigned int>())
            pulse_solenoid(cmd["pulseSV"], cmd["pulseTime"]);
        else if(cmd["pulseSV"].is<JsonArray>())
            pulse_solenoids(cmd["pulseSV"],  cmd["pulseTime"]);
    }
    if(cmd.containsKey("fireEM")){
        if(cmd["fireEM"].is<unsigned int>())
            fire_ematch(cmd["fireEM"]);
        else if(cmd["fireEM"].is<JsonArray>())
            fire_ematches(cmd["fireEM"]);
    }
    if(cmd.containsKey("waitThen")&&cmd.containsKey("waitTime")){
        wait_then(cmd["waitThen"], cmd["waitTime"]);
    }
    if(cmd.containsKey("startSQ")){
        start_seq(cmd["startSQ"]);
    }
    if(cmd.containsKey("stopSQ")){
        stop_seq(cmd["stopSQ"]);
    }
    if(cmd.containsKey("pauseSQ")){
        pause_seq(cmd["pauseSQ"]);
    }
    if(cmd.containsKey("playSQ")){
        play_seq(cmd["playSQ"]);
    }
    if(cmd.containsKey("userCmd")){
        if(cmd["userCmd"].is<const char*>()){
            const char* usercmd = cmd["userCmd"];
            if(usercmds.containsKey(usercmd)){
                if(usercmds[usercmd].is<JsonObject>())
                    process_cmd_json(usercmds[usercmd]);
                else if(usercmds[usercmd].is<JsonArray>())
                    process_cmd_array(usercmds[usercmd]);
            }
        }
    }
};

void RXTask::pulse_solenoid(uint8_t sol_ch, uint16_t pulse_dur){
    xTimerChangePeriod(pulseTimers[sol_ch-1], pulse_dur, NEVER); // set new pulse period
    open_solenoid(sol_ch); // open solenoid
    xTimerStart(pulseTimers[sol_ch-1], NEVER); // start the timer to close this solenoid
};

void RXTask::pulse_solenoids(JsonArrayConst sol_ch, uint16_t pulse_dur){
    uint8_t num_ch = sol_ch.size();
    for(uint8_t i = 0; i < num_ch; i++)
        xTimerChangePeriod(pulseTimers[(uint8_t)sol_ch[i]-1], pulse_dur, NEVER); // set new pulse period
    open_solenoids(sol_ch); // open solenoids
    for(uint8_t i = 0; i < num_ch; i++)
        xTimerStart(pulseTimers[(uint8_t)sol_ch[i]-1], NEVER); // start the timer to close this solenoid
};

void RXTask::open_solenoid(uint8_t sol_ch){
    if(sol_ch >= 1 && sol_ch<=NUM_SOLENOIDS)
        sys.statedata.setSolenoidState(sol_ch-1, OPEN, true);
};

void RXTask::open_solenoids(JsonArrayConst sol_ch){
    uint8_t num_ch = sol_ch.size();
    for(uint8_t i = 0; i < num_ch; i++)
        if(sol_ch >= 1 && sol_ch<=NUM_SOLENOIDS && sol_ch[i].is<unsigned int>()){
            uint8_t this_ch = sol_ch[i];
            sys.statedata.setSolenoidState(this_ch-1, OPEN, i==(num_ch-1)); // update all solenoid states before flagging ValveTask
        }
};

void RXTask::close_solenoid(uint8_t sol_ch){
    if(sol_ch >= 1 && sol_ch<=NUM_SOLENOIDS)
        sys.statedata.setSolenoidState(sol_ch-1, CLOSED); // update all solenoid states before flagging ValveTask
};

void RXTask::close_solenoids(JsonArrayConst sol_ch){
    uint8_t num_ch = sol_ch.size();
    for(uint8_t i = 0; i < num_ch; i++)
        if(sol_ch[i] >= 1 && sol_ch[i]<=NUM_SOLENOIDS && sol_ch[i].is<unsigned int>()){
            uint8_t this_ch = sol_ch[i];
            sys.statedata.setSolenoidState(this_ch-1, CLOSED, i==(num_ch-1));
        }
};

void RXTask::_close_solenoid(TimerHandle_t xTimer){
    xTimerStop(xTimer, NEVER); // reset timer so its no longer active
    uint8_t sol_ch[4];
    sol_ch[0] = (long) pvTimerGetTimerID(xTimer);
    close_solenoid(sol_ch[0]+1);
};

void RXTask::fire_ematch(uint8_t em_ch){
    if(em_ch >= 1 && em_ch<=NUM_SOLENOIDS)
        sys.statedata.fireEmatch(em_ch-1);
};

void RXTask::fire_ematches(JsonArrayConst em_ch){
    uint8_t num_ch = em_ch.size();
    for(uint8_t i = 0; i < num_ch; i++)
        if(em_ch[i].is<unsigned int>()){
            uint8_t this_ch = em_ch[i];
            sys.statedata.fireEmatch(this_ch-1, i==(num_ch-1));
        }
};

void RXTask::wait_then(JsonObjectConst cmd, uint16_t wait_time){
    // TODO: check that wait timer isn't already in use
    wait_cmd.clear();
    wait_cmd.set(cmd);
    xTimerChangePeriod(waitTimer, wait_time, NEVER); // set wait time
    xTimerStart(waitTimer, NEVER); // start the timer to close this solenoid
};

void RXTask::wait_callback(TimerHandle_t xTimer){
    xTimerStop(waitTimer, NEVER);
    sys.tasks.rxtask.sendcmdJSON(wait_cmd.as<JsonObject>()); // on callback, send the command to the queue
    // can't call the command directly here without creating huge timer callback stacks
};

void RXTask::start_seq(const char* seq_name){
    sys.tasks.sequencetask.start_seq();
};

void RXTask::stop_seq(const char* seq_name){
    sys.tasks.sequencetask.stop_seq();
};

void RXTask::pause_seq(const char* seq_name){
    sys.tasks.sequencetask.pause_seq();
};

void RXTask::play_seq(const char* seq_name){
    sys.tasks.sequencetask.play_seq();
};

void RXTask::load_seq(const char* seq_name){
    sys.tasks.sequencetask.load_seq(seq_name);
};

void RXTask::delay_seq(uint16_t ticks_to_wait){
    sys.tasks.sequencetask.delay_seq(ticks_to_wait);
};


void RXTask::sendcmd(const char* cmd){
    StaticJsonDocument<MAX_CMD_LENGTH> cmdJSON;
    DeserializationError ret = deserializeJson(cmdJSON, cmd);
    if(ret == DeserializationError::Ok){ // if successfully deserialized json
        sendcmdJSON(cmdJSON);
    }
    else{
        Serial.print("Bad command: ");
        Serial.println((char*)cmd);
    }
};

void RXTask::sendcmdJSON(StaticJsonDocument<MAX_CMD_LENGTH> cmd){
    cmd_t buf;
    buf.doc.set(cmd);
    cmdbuf.send(buf);
};

void RXTask::readInput(){
    #ifdef RADIO_TXRX
        while(sys.tasks.radiotask.packetAvailable()){
            packet_t packet_in;
            sys.tasks.radiotask.waitForPacket(packet_in); 
            sendcmd((char*)packet_in.data);
        }
    #endif
    while(Serial.available()){
        char packet_in[MAX_CMD_LENGTH];
        size_t chars_read = Serial.readBytesUntil('\n',packet_in, MAX_CMD_LENGTH); // endline indicates end of a json cmd
        if(chars_read > 0 && chars_read < MAX_CMD_LENGTH){
            sendcmd(packet_in); 
        }
    }
};