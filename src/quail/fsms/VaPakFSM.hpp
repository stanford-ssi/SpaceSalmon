#pragma once

#include "Task.hpp"
#include "../../slate/slate_utils.h"
#include "../../slate/slate_enums.h"
#include "TankGeneric.hpp"

class VaPakFSM : public TankGeneric {
    public:
        VaPakFSM(uint8_t priority);
        void activity() override;

    private:
        bool inBounds() override;
        SlateField<float> weight = 0;
};