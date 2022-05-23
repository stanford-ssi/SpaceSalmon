#include "ArmingTask.hpp"

ArmingTask::ArmingTask(uint8_t priority) : Task(priority, "Arming"){}

void ArmingTask::activity(){
    while(true){
        packet_t rx_pkt;
        sys.tasks.radio.waitForPacket(rx_pkt);
        
        if(rx_pkt.data[0] == 'A'){
            // arm COTS (persistently)
	        digitalWrite(36, HIGH);
            vTaskDelay(100);
            digitalWrite(36, LOW);
            // 
            sys.armed.post(true);

        }else if (rx_pkt.data[0] == 'D'){
            // disarm COTS
            digitalWrite(35, HIGH);
            vTaskDelay(100);
            digitalWrite(35, LOW);
            // disarm squib driver
            sys.armed.post(false);
        }
    }
}