#pragma once

#include "Task.hpp"
#include "../../slate/slate_utils.h"
#include "../../slate/slate_enums.h"
#include "../config.h"
#include "main.hpp"

class TankGeneric : public Task<500> {
    public:
        TankGeneric(uint8_t priority, 
            SlateField<TankState>& state, 
            SlateField<float>& op_press, 
            SlateField<float>& up_press, 
            SlateField<float>& down_press, 
            SlateField<bool>& fill_sol, 
            SlateField<bool>& vent_sol, 
            SlateField<bool>& bleed_sol) : 
            Task(priority, "Tank"), state(state), op_press(op_press), up_press(up_press), down_press(down_press), fill_sol(fill_sol), vent_sol(vent_sol), bleed_sol(bleed_sol) {};
        virtual void activity() { return; /** ask my kids **/};

    protected:
        const int MAWP = 700 * PSI_TO_PA;
        const TickType_t FSM_FREQ = 50;
        const TickType_t PULSE_FREQ = 10;

        SlateField<TankState>& state;
        SlateField<float>& op_press;
        SlateField<float>& up_press;
        SlateField<float>& down_press;
        SlateField<float>& mass = sys.telem_slate.load_mass;
        SlateField<float>& mass_offset = sys.telem_slate.load_mass_offset;
        SlateField<bool>& fill_sol;
        SlateField<bool>& vent_sol;
        SlateField<bool>& bleed_sol;

        TickType_t lastTick = 0;

        virtual bool inBounds() { return false; /** ask my kids **/};
        float ZERO_WEIGHT = 0;
        float ZERO_PRESS = 0;
};