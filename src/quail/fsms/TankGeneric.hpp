#pragma once

#include "Task.hpp"
#include "../../slate/slate_utils.h"
#include "../../slate/slate_enums.h"
#include "../config.h"

class TankGeneric : public Task<500> {
    public:
        TankGeneric(uint8_t priority, 
            SlateField<TankState>& state, 
            SlateField<float>& op, 
            SlateField<float>& press, 
            SlateField<float>& weight, 
            SlateField<bool>& fill_sol, 
            SlateField<bool>& vent_sol, 
            SlateField<bool>& bleed_sol) : 
            Task(priority, "Tank"), state(state), op(op), press(press), weight(weight), fill_sol(fill_sol), vent_sol(vent_sol), bleed_sol(bleed_sol) {};
        virtual void activity() { return; /** ask my kids **/};

    protected:
        const int MAWP = 700 * PSI_TO_PA;
        const TickType_t FSM_FREQ = 50;

        SlateField<TankState>& state;
        SlateField<float>& op;
        SlateField<float>& press;
        SlateField<float>& weight;
        SlateField<bool>& fill_sol;
        SlateField<bool>& vent_sol;
        SlateField<bool>& bleed_sol;

        TickType_t lastTick = 0;

        virtual bool inBounds() { return false; /** ask my kids **/};
        float ZERO_WEIGHT = 0;
        float ZERO_PRESS = 0;
};