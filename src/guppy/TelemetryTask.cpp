#include "TelemetryTask.hpp"

TelemetryTask::TelemetryTask(uint8_t priority) : Task(priority, "Telem"){}

void TelemetryTask::activity(){
    while(true){
        vTaskDelay(1000);
        
        gps_data_t data = sys.tasks.gps.locationData;
        OneBattery::cell_voltage_t batt = sys.tasks.alt.battData;

        StaticJsonDocument<1000> doc;
        doc["lat"] = data.lat;
        doc["lon"] = data.lon;
        doc["alt"] = data.alt;
        doc["bat"] = batt.cellMain;
        char str[200];
        serializeJson(doc, str, 200);

        packet_t pkt;
        strcpy((char*)pkt.data, str);
        pkt.len = strlen(str);
        
        sys.tasks.radio.sendPacket(pkt);
    }
}