#include "TXTask.hpp"
#include "main.hpp"

TXTask::TXTask(uint8_t priority, uint16_t tx_interval_ms):
Task(priority, "TX"), tx_interval_ms(tx_interval_ms){};

void TXTask::activity() {
    TickType_t lastSensorTime = xTaskGetTickCount();
    uint8_t i = 0;
    uint8_t j = 0;
    while(true){
        i++;
        j++;
        vTaskDelayUntil(&lastSensorTime, tx_interval_ms);
        // get state JSON
        StaticJsonDocument<1024>* stateJSON = sys.statedata.getState(); // updates and returns pointer to state JSON
        // log
        // sys.tasks.logger.log(*stateJSON);    

        if(i == LOG_FACTOR){
            i = 0;
            size_t len = measureJson(*stateJSON);
            char MsgPackstr[len + 5]; //create char buffer with space
            serializeJson(*stateJSON, MsgPackstr, sizeof(MsgPackstr));        
            // if at tx_interval, write over selected TX
            writeUSB(MsgPackstr);
            #ifdef RADIO_TXRX
            if(j == RADIO_FACTOR*LOG_FACTOR){ // if at a radio transmission interval
                packet_t pkt; // create radio packet type
                memcpy(pkt.data, &MsgPackstr, sizeof(MsgPackstr)); // copy state data into packet
                pkt.len = sizeof(MsgPackstr); // set packet size
                sys.tasks.radiotask.sendPacket(pkt); // add packet to radio transmission queue
                j = 0;
                sys.statedata.clearError(); // clear error after sending over radio
            }
            #else
                sys.statedata.clearError(); // clear state data after sending once
            #endif
        }
    }
};

void TXTask::writeUSB(const char* msg){
    // Serial.println is asynchronous so long as we don't over-fill the Serial buffer
    Serial.println(msg); // in theory, may want to consider checking buffer space first
};