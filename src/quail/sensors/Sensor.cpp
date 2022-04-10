#include "Sensor.hpp"
#include "main.hpp"

uint8_t Sensor::num_sensors = 0;
uint8_t Sensor::num_cfgs = 0;

Sensor::Sensor(const char* ch_name, Ad7124::InputSel ainp, Ad7124::InputSel ainm, SensorSlate& slate) :
ch_name(ch_name), ch_id(num_sensors), ainp(ainp), ainm(ainm), slate(slate), tick(sys.slate.board.tick)
{num_sensors++;} 

void Sensor::updateSlate() {
    unsigned currTick = tick();
    unsigned dt = currTick - lastTick;

    window += slate.val();

    slate.avg << window.avg();
    slate.dif << window.delta() / dt;
    slate.ntg << slate.ntg() + window.peek() * dt;

    lastTick = currTick;
}