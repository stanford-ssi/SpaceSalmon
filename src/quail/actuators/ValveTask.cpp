#include "ValveTask.hpp"
#include "main.hpp"

ValveTask::ValveTask(uint8_t priority)
    : Task(priority, "Valves"),
      valve1(sys.telem_slate.fuel_main, sys.telem_slate.fuel_main_pulse, 22),
      valve2(sys.telem_slate.oxdz_main, sys.telem_slate.oxdz_main_pulse, 23),
      valve3(sys.telem_slate.fuel_fill, sys.telem_slate.fuel_fill_pulse, 24),
      valve4(sys.telem_slate.fuel_bleed, sys.telem_slate.fuel_bleed_pulse, 25),
      valve5(sys.telem_slate.oxdz_fill, sys.telem_slate.oxdz_fill_pulse, 26),
      valve6(sys.telem_slate.oxdz_bleed, sys.telem_slate.oxdz_bleed_pulse, 27),
      valve7(sys.telem_slate.oxdz_vent, sys.telem_slate.oxdz_vent_pulse, 28),
      valve8(sys.telem_slate.s8, sys.telem_slate.s8_pulse, 29),
      valve9(sys.telem_slate.s9, sys.telem_slate.s9_pulse, 30),
      valve10(sys.telem_slate.s10, sys.telem_slate.s10_pulse, 31),
      valve11(sys.telem_slate.s11, sys.telem_slate.s11_pulse, 32),
      valve12(sys.telem_slate.danger, sys.telem_slate.s12_pulse, 33) {}