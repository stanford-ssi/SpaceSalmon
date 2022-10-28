#include "ValveTask.hpp"
#include "main.hpp"

ValveTask::ValveTask(uint8_t priority)
    : Task(priority, "Valves"),
      valve1(sys.telem_slate.s1, sys.telem_slate.s1_pulse, 22),
      valve2(sys.telem_slate.s2, sys.telem_slate.s2_pulse, 23),
      valve3(sys.telem_slate.s3, sys.telem_slate.s3_pulse, 24),
      valve4(sys.telem_slate.s4, sys.telem_slate.s4_pulse, 25),
      valve5(sys.telem_slate.s5, sys.telem_slate.s5_pulse, 26),
      valve6(sys.telem_slate.s6, sys.telem_slate.s6_pulse, 27),
      valve7(sys.telem_slate.s7, sys.telem_slate.s7_pulse, 28),
      valve8(sys.telem_slate.s8, sys.telem_slate.s8_pulse, 29),
      valve9(sys.telem_slate.s9, sys.telem_slate.s9_pulse, 30),
      valve10(sys.telem_slate.s10, sys.telem_slate.s10_pulse, 31),
      valve11(sys.telem_slate.s11, sys.telem_slate.s11_pulse, 32),
      valve12(sys.telem_slate.s12, sys.telem_slate.s12_pulse, 33) {}