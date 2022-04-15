#pragma once

#include "TXTask.hpp"
#include "main.hpp"
#include <rBase64.h>

TXTask::TXTask(uint8_t priority, uint16_t tx_interval_ms):
Task(priority, "TX"), tx_interval_ms(tx_interval_ms){};

void TXTask::activity() {
    vTaskDelay(2000);

    TickType_t lastSensorTime = xTaskGetTickCount();
    uint8_t i = 0;
    uint8_t j = 0;
    while(true){
        i++;
        j++;
        vTaskDelayUntil(&lastSensorTime, tx_interval_ms);
        // get state JSON
        sys.slate.board.tick << xTaskGetTickCount();
        sys.slate.board.logging << sys.tasks.logger.isLoggingEnabled();
        // log

        StaticJsonDocument<2048> slateJSON;

        //char string[1024];
        JsonVariant variant = slateJSON.to<JsonVariant>();
        sys.slate.dump(variant);
        //size_t pkt = serializeMsgPack(slateJSON, string, 1024);

        // sys.tasks.logger.log(slateJSON);    

        if(i == LOG_FACTOR){
            sys.tasks.ethernettask.send(slateJSON);

            i = 0;
            size_t len = measureJson(slateJSON);
            char MsgPackstr[len + 5]; //create char buffer with space
            serializeJson(slateJSON, MsgPackstr, sizeof(MsgPackstr));        
            // if at tx_interval, write over selected TX
            writeUSB(MsgPackstr);
            #ifdef RADIO_TXRX
            if(j == RADIO_FACTOR*LOG_FACTOR){ // if at a radio transmission interval
                packet_t pkt; // create radio packet type
                memcpy(pkt.data, &MsgPackstr, sizeof(MsgPackstr)); // copy slate data into packet
                pkt.len = sizeof(MsgPackstr); // set packet size
                sys.tasks.radiotask.sendPacket(pkt); // add packet to radio transmission queue
                j = 0;
                sys.slate.error = 0; // clear error after sending over radio
                                     // TODO clean this up using the ErrorType enum
            }
            #else
            sys.slate.board.error << 0;
            #endif
        }
    }
};

void TXTask::writeUSB(const char* msg){
    // Serial.println is asynchronous so long as we don't over-fill the Serial buffer
    Serial.println(msg); // in theory, may want to consider checking buffer space first
    Serial.flush();
};