#include "main.hpp"

void TestTask::activity()
{
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);

    while (true)
    {
        vTaskDelay(6000);

        digitalWrite(LED_BUILTIN, HIGH);

        Serial.println("Test!");

        //sys.slate.solenoids[1] = true;
        //float f = sys.slate.adc[0];
        sys.slate.arm = true;

        Serial.println("hmm");

        sys.slate.fire = true;

        Serial.println(sys.slate.solenoid[0].id.c_str());
        Serial.println(sys.slate.solenoid[1].id.c_str());
        Serial.println(sys.slate.solenoid[2].id.c_str());
        Serial.println(sys.slate.solenoid[3].id.c_str());
        Serial.println(sys.slate.solenoid[4].id.c_str());



        Serial.println("OK");

        Serial.println(sys.slate.id.c_str());
        Serial.println(sys.slate.arm.id.c_str());
        Serial.println((uint32_t)&sys.slate.arm, HEX);
        Serial.println((uint32_t)&(std::ref(sys.slate.arm).get()), HEX);

        vTaskDelay(10);
        StaticJsonDocument<1000> doc;
        vTaskDelay(10);
        JsonVariant variant = doc.to<JsonVariant>();
        vTaskDelay(10);
        sys.slate.solenoid.dump(variant);
        vTaskDelay(10);
        serializeJson(doc, Serial);
        vTaskDelay(10);

        Serial.println();
        Serial.println("Done");
    }
};