#include "RXTask.hpp"
#include "main.hpp"
#include <rBase64.h>
#include <timers.h>

RXTask::RXTask(uint8_t priority, uint16_t rx_interval):Task(priority, "RX"), rx_interval_ms(rx_interval){
    commTimer = xTimerCreateStatic(
        "Communication Timeout Reset TImer",
        COMM_RESET, // default time is 5 mins
        false, // timer reaching zero triggers reset, no need for reload
        0, // timer id, only one timer for callback so unimportant
        [](TimerHandle_t xTimer){sys.slate.board.comms << false;}, // callback as lambda for succinctness
        &(commTimerBuf)
    );
    xTimerStart(commTimer, NEVER);
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
            sys.slate << buf.doc.as<JsonVariant>();
        }
        vTaskDelayUntil(&lastSensorTime, rx_interval_ms); // wait for a while to allow other tasks time to operate
    }
};

void RXTask::sendcmd(const char* cmd){
    StaticJsonDocument<MAX_CMD_LENGTH> cmdJSON;
    DeserializationError ret = deserializeJson(cmdJSON, cmd);

    if(ret == DeserializationError::Ok){ // if successfully deserialized json
        sendcmdJSON(cmdJSON);
    } else{
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
    // Should always receive the format {id:<some string>, data: <base64-encoded-string parsable as JSON>}
    #ifdef RADIO_TXRX
        while(sys.tasks.radiotask.packetAvailable()){
            xTimerReset(commTimer, NEVER);
            packet_t packet_in;
            sys.tasks.radiotask.waitForPacket(packet_in); 
            sendcmd((char*)packet_in.data);
        }
    #endif
    while(Serial.available()){
        xTimerReset(commTimer, NEVER);
        char packet_in[MAX_CMD_LENGTH];
        size_t chars_read = Serial.readBytesUntil('\n',packet_in, MAX_CMD_LENGTH); // endline indicates end of a json cmd
        if(chars_read > 0 && chars_read < MAX_CMD_LENGTH){
            StaticJsonDocument<MAX_CMD_LENGTH> doc;
            DeserializationError ret = deserializeJson(doc, packet_in);
            if(ret == DeserializationError::Ok){ // if successfully deserialized json
                JsonVariant cmd = doc["cmd"];
                if(!cmd.isNull()){
                    char out[MAX_CMD_LENGTH];
                    char temp[MAX_CMD_LENGTH];
                    strcpy(temp, cmd.as<char *>());
                    rbase64_decode(out, temp, strlen(temp)); // binary sent over serial has been decoded from base64
                    sendcmd(out);
                }
            }
        }
    }
    #ifdef ETHERNET_TXRX
        while(sys.tasks.ethernettask.cmdAvailable()) {
            xTimerReset(commTimer, NEVER);

            char cmdStr[MAX_CMD_LENGTH];
            sys.tasks.ethernettask.waitForCmd(cmdStr);

            StaticJsonDocument<MAX_CMD_LENGTH> doc;
            DeserializationError ret = deserializeJson(doc, cmdStr);
            JsonVariant cmdJson = doc["cmd"];
            char out[MAX_CMD_LENGTH];
            serializeJson(cmdJson, out);
            sendcmd(out);
        } 
    #endif
};