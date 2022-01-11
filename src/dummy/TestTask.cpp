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

        uint32_t time = xTaskGetTickCount();

        for (uint32_t i = 0; i < 1000; i++)
        {
            StaticJsonDocument<1000> doc;
            char string[1000];
            JsonVariant variant = doc.to<JsonVariant>();
            sys.slate.dump(variant);
            size_t pkt = serializeMsgPack(doc,string,1000);
            
        }

        Serial.println(xTaskGetTickCount() - time);
        // Serial.println("Done");
    }
};

// some benchmarks:
// - dump and print to USB (unbuffered): 1400us
// - dump and print to USB (buffered): 389us
// - dump: 223us
// - dump with no mutex: 139us
// - dump with msgpack: 176us