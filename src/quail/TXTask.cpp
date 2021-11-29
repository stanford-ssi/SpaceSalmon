#include "TXTask.hpp"
#include "main.hpp"

TXTask::TXTask(uint8_t priority, uint16_t tx_interval_ms):
Task(priority, "TX"), tx_interval_ms(tx_interval_ms){};

void TXTask::activity() {
    TickType_t lastSensorTime = xTaskGetTickCount();
    uint8_t i = 0;
    while(true){
        i++;
        vTaskDelayUntil(&lastSensorTime, tx_interval_ms/LOG_FACTOR);
        // get state JSON
        StaticJsonDocument<1024>* stateJSON = sys.statedata.getState(); // updates and returns pointer to state JSON
        // serialize into str
        size_t len = measureJson(*stateJSON);
        char str[len + 5]; //create char buffer with space
        serializeJson(*stateJSON, str, sizeof(str));
        // log
        // sys.tasks.logger.log(str); // adds to queue to be written to SD card
        // if at tx_interval, write over selected TX
        if(i == LOG_FACTOR){
            send(str);
            i = 0;
        }
    }
};

void TXTask::send(const char* msg){
    #ifdef RADIO_TXRX
                packet_t pkt; // create radio packet type
                memcpy(pkt.data, &msg, sizeof(msg)); // copy state data into packet
                pkt.len = sizeof(msg); // set packet size
                sys.tasks.radiotask.sendPacket(pkt); // add packet to radio transmission queue
    #else
        writeUSB(msg); // add state data to Serial transmission queue
    #endif
};

void TXTask::send(JsonDocument &jsonDoc){
    size_t len = measureJson(jsonDoc);
    char str[len + 5]; //create char buffer with space
    serializeJson(jsonDoc, str, sizeof(str));
    send(str);
}

void TXTask::writeUSB(const char* msg){
    // Serial.println is asynchronous so long as we don't over-fill the Serial buffer
    Serial.println(msg); // in theory, may want to consider checking buffer space first
};