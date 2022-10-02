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

        telemetry_t::telemetry_pkt_t packet;
        sys.telem_slate.seralize(packet);
        
        //sys.tasks.logger.log(json_buffer);
        //writeUSB(json_buffer);
        
        sys.slateServer.send(packet.payload, sizeof(packet.payload));
        
    }
};

void TXTask::writeUSB(const char* msg){
    // we should use printf for this... 
    // Also there should be a centralized console for low-rate unstructure data.

    // Serial.println is asynchronous so long as we don't over-fill the Serial buffer
    Serial.println(msg); // in theory, may want to consider checking buffer space first
    Serial.flush();
};