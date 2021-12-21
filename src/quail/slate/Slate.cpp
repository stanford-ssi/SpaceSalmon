#include "Slate.hpp"

Slate slate;

void mains(){
    
    slate.solenoids.sol1.set(true);
    slate.adc[0].get();
    slate.squib.arm.set(true);
    slate.squib.fire[0].set(true);

}