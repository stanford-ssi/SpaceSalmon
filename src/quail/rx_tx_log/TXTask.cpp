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

        // still sending data so no hangups -> we prevent from failsafe rebooting
        Watchdog.reset();

        // prepare slate
        sys.telem_slate.tick.set(xTaskGetTickCount());
        sys.telem_slate.logging.set(sys.tasks.logger.isLoggingEnabled());

        // convert slate to json
        StaticJsonDocument<DATA_PCKT_LEN> slateJSON;
        JsonVariant variant = slateJSON.to<JsonVariant>();
        variant.memoryUsage();
        //sys.slate >> variant;
        
        // always 

        if(i == LOG_FACTOR){
            i = 0;

            size_t ser_len = serializeJson(slateJSON, json_buffer, sizeof(json_buffer));   

            sys.tasks.logger.log(json_buffer);

            //writeUSB(json_buffer);
            
            #ifdef ETHERNET_TXRX
                sys.tasks.ethernettask.send(json_buffer, ser_len);
            #endif 
            #ifdef RADIO_TXRX
                if(j == RADIO_FACTOR*LOG_FACTOR){ // if at a radio transmission interval
                    packet_t pkt; // create radio packet type
                    memcpy(pkt.data, &MsgPackstr, sizeof(MsgPackstr)); // copy slate data into packet
                    pkt.len = sizeof(MsgPackstr); // set packet size
                    sys.tasks.radiotask.sendPacket(pkt); // add packet to radio transmission queue
                    j = 0;
                    sys.slate.board.error << 0; // clear error after sending over radio
                                        // TODO clean this up using the ErrorType enum
                }
            #else
            #endif
        }
    }
};

void TXTask::writeUSB(const char* msg){
    // Serial.println is asynchronous so long as we don't over-fill the Serial buffer
    Serial.println(msg); // in theory, may want to consider checking buffer space first
    Serial.flush();
};