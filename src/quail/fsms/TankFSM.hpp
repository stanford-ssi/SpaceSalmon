#pragma once

#include "Task.hpp"
#include "../../slate/slate_utils.h"
#include "../../slate/slate_enums.h"
#include "TankGeneric.hpp"


class TankFSM : public TankGeneric {
    public:
        TankFSM(uint8_t priority,
            SlateField<TankState>& state, 
            SlateField<float>& op_press,
            SlateField<float>& press,
            SlateField<bool>& fill_sol,
            SlateField<bool>& vent_sol,
            SlateField<bool>& bleed_sol);
        TankFSM(uint8_t priority);
        void activity() override;

    private:
        bool inBounds() override;
};