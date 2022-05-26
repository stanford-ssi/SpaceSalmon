#pragma once

#include "Task.hpp"
#include "../slate/SlateEndpoints.hpp"
#include "../slate/SlateAbstractions.hpp"
#include "TankGeneric.hpp"


class VaPakFSM : public TankGeneric {
    public:
        using TankGeneric::TankGeneric;
        void activity() override;

    private:
        TickType_t lastBoundsCheck = 0;
        bool inBounds() override;
};