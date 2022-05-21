#include "TelemetryTask.hpp"
#include "Packet.hpp"
#include "compression_utils.h"

TelemetryTask::TelemetryTask(uint8_t priority) : Task(priority, "Telem") {}

const char *str(FlightState state)
{
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

void TelemetryTask::activity()
{
    while (true)
    {
        vTaskDelay(1000);

        if (sys.tasks.radio.isIdle())
        {
            //gps_data_t data = sys.tasks.gps.locationData;
            OneBattery::cell_voltage_t batt = sys.tasks.alt.battData;

            rf_down_t down_packet;

            down_packet.time = trimBits(xTaskGetTickCount() / 1000, 18); //seconds mod 72 hours

            down_packet.filter_alt = compressFloat(sys.tasks.filter.filter.p_alt, -2000.0, 40000.0, 15);
            down_packet.filter_vel = compressFloat(sys.tasks.filter.filter.p_vel, -1000.0, 1000.0, 11);

            down_packet.battery = compressFloat(batt.cellMain, 1.0, 6.0, 8);
            down_packet.lat = 5; // compressFloat(data.lat, 0.0, 10.0, 18);
            down_packet.lon = 5; // compressFloat(data.lon, 0.0, 10.0, 18);
            down_packet.gps_alt = 5; // compressFloat(data.alt, -2000.0, 40000.0, 15);

            down_packet.gps_lock = 0; // data.valid;
            down_packet.armed = sys.armed;
            down_packet.state = sys.tasks.filter.plan.p_state;

            down_packet.pyroA = sys.pyro.getStatus(Pyro::SquibA);
            down_packet.pyroB = sys.pyro.getStatus(Pyro::SquibB);

            down_packet.pyroA_fired = sys.tasks.filter.plan.pyroA_fired;
            down_packet.pyroB_fired = sys.tasks.filter.plan.pyroB_fired;
            
            packet_t pkt;
            memcpy(pkt.data, &down_packet, sizeof(down_packet));
            pkt.len = sizeof(down_packet);

            sys.tasks.radio.sendPacket(pkt);
        }
    }
}