#include "SolenoidTask.hpp"
#include "main.hpp"

SolenoidTask::SolenoidTask(uint8_t priority, uint8_t firstSVpin, uint8_t lastSVpin): 
Task(priority, "Solenoid"), firstSVpin(firstSVpin), lastSVpin(lastSVpin) {
    evgroup = xEventGroupCreateStatic(&evbuf);
};

void SolenoidTask::activity(){
    while(true){
        uint32_t flags = xEventGroupWaitBits(evgroup, SV_STATE_UPDATED, true, false, NEVER); // wait for solenoid state to be updated
        // get solenoid states
        uint8_t SVstate = sys.statedata.getSolenoidState();
        for(uint8_t i = 0; i <= (lastSVpin-firstSVpin); i++) { // go thru all solenoids and update based on current state
            if(((SVstate >> i)&0b1) == OPEN) { // mask all but bit of interest and compare
                // open solenoid i
            }
            else {
                // close solenoid i
            }
        }
    }
};

void SolenoidTask::update_SV(){
    xEventGroupSetBits(evgroup, SV_STATE_UPDATED);
};