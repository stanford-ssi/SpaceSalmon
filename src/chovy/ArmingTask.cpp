#include "ArmingTask.hpp"

ArmingTask::ArmingTask(uint8_t priority) : Task(priority, "Arming"){}

void ArmingTask::activity(){
    while(true){
        packet_t rx_pkt;
        sys.tasks.radio.waitForPacket(rx_pkt);

        if(rx_pkt.data[0] == 'A'){
            // arm COTS (persistently)
	        digitalWrite(33, HIGH);
            vTaskDelay(100);
            digitalWrite(33, LOW);
            // 
            sys.armed.post(true);

        }else if (rx_pkt.data[0] == 'D'){
            // disarm COTS
            digitalWrite(32, HIGH);
            vTaskDelay(100);
            digitalWrite(32, LOW);
            // disarm squib driver
            sys.armed.post(false);
        }
    }
}