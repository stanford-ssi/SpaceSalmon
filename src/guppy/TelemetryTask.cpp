#include "TelemetryTask.hpp"

TelemetryTask::TelemetryTask(uint8_t priority) : Task(priority, "Telem"){}

const char* str(FlightState state){
    switch (state)
    {
    case Waiting:
        return "Waiting";
    case Flight:
        return "Flight";
    case Falling:
        return "Falling";
    case Landed:
        return "Landed";
    default:
        return "?";
    }
}

void TelemetryTask::activity(){
    while(true){
        vTaskDelay(1000);
        
        gps_data_t data = sys.tasks.gps.locationData;
        OneBattery::cell_voltage_t batt = sys.tasks.alt.battData;

        StaticJsonDocument<1000> doc;
        doc["time"] = xTaskGetTickCount();
        doc["lat"] = data.lat;
        doc["lon"] = data.lon;
        doc["gps_alt"] = data.alt;
        doc["bat"] = batt.cellMain;
        doc["state"] = str(sys.tasks.filter.plan.p_state);
        doc["alt"] = (float) sys.tasks.filter.filter.p_alt;
        doc["vel"] = (float) sys.tasks.filter.filter.p_vel;
        doc["arm"] = (bool) sys.armed;
        char str[200];
        serializeJson(doc, str, 200);

        packet_t pkt;
        strcpy((char*)pkt.data, str);
        pkt.len = strlen(str);
        
        sys.tasks.radio.sendPacket(pkt);
    }
}