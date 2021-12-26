#include "Slate.hpp"

Slate slate;

void mains(){
    
    slate.solenoids.sol1 = true;
    float f = slate.adc[0];
    slate.squib.arm = true;
    slate.squib.fire[0] = true;

}