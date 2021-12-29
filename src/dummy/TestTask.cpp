#include "main.hpp"

void TestTask::activity()
{
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);

    while (true)
    {
        vTaskDelay(6000);

        digitalWrite(LED_BUILTIN, HIGH);

        // Serial.println("Test!");

        sys.slate.solenoid[1] = true;
        float f = sys.slate.adc_in[0];
        sys.slate.squib.arm = true;

        // Serial.println("hmm");

        // Serial.println();

        // Serial.println(sys.slate.solenoid[0].originalptr , HEX);
        // Serial.println((uint32_t)&sys.slate.solenoid[0], HEX);

        // Serial.println(sys.slate.solenoid[4].originalptr , HEX);
        // Serial.println((uint32_t)&sys.slate.solenoid[4], HEX);

        // Serial.println(sys.slate.id.c_str());
        // Serial.println(sys.slate.arm.id.c_str());
        // Serial.println((uint32_t)&sys.slate.arm, HEX);
        // Serial.println((uint32_t)&(std::ref(sys.slate.arm).get()), HEX);

        vTaskDelay(10);
        StaticJsonDocument<1000> doc;
        vTaskDelay(10);
        JsonVariant variant = doc.to<JsonVariant>();
        vTaskDelay(10);
        sys.slate.dump(variant);
        vTaskDelay(10);
        serializeJson(doc, Serial);
        vTaskDelay(10);

        Serial.println();
        // Serial.println("Done");
    }
};