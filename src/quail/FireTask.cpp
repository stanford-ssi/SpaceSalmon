#include "FireTask.hpp"

FireTask::FireTask(uint8_t priority) : Task(priority, "Fire"), squib1(sys.squib_spi,20){};

void FireTask::activity()
{
    
    uint8_t send = 0x96;
	uint8_t recv = 0x00;
    sys.squib_spi.begin();
    pinMode(20,OUTPUT);
    digitalWrite(20,HIGH);

    vTaskDelay(6000);
    
    digitalWrite(20,LOW);
    recv = sys.squib_spi.transfer(send);
    digitalWrite(20,HIGH);
	Serial.println(recv);
    while (true)
    {
        vTaskDelay(500);
        Serial.println("done");
    }
    
    // squib1.Init();
    // while (true)
    // {
    //     digitalWrite(2,HIGH);
    //     squib1.fire(Squib::Fire_t::CMD_FIRE_1A2A1B2B);
    //     vTaskDelay(500);
    //     digitalWrite(2,LOW);
    //     squib1.fire(Squib::Fire_t::CMD_FIRE_NO_SQUIBS);
    //     vTaskDelay(500);
    // }
}