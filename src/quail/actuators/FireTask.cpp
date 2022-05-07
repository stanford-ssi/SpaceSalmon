#include "FireTask.hpp"
#include "main.hpp"

FireTask::FireTask(uint8_t priority, uint8_t Squib1_SS, uint8_t Squib2_SS) : 
Task(priority, "Fire"), squib1(sys.squib_spi,Squib1_SS), squib2(sys.squib_spi,Squib2_SS), slate(sys.slate.squib)
{
    squibManager = xEventGroupCreateStatic(&evbuf);

    ch_map[0] = {&squib1, CMD_FIRE_1B};
    ch_map[1] = {&squib1, CMD_FIRE_2B};
    ch_map[2] = {&squib1, CMD_FIRE_2A};
    ch_map[3] = {&squib1, CMD_FIRE_1A};
    ch_map[4] = {&squib2, CMD_FIRE_1B};
    ch_map[5] = {&squib2, CMD_FIRE_2B};
    ch_map[6] = {&squib2, CMD_FIRE_2A};
    ch_map[7] = {&squib2, CMD_FIRE_1A};
};

void FireTask::activity()
{
    sys.squib_spi.begin();
    squib1.Init();
    // squib2.Init(); don't uncomment. i dont know why but shits fucked
    while(true) {
        xEventGroupWaitBits(squibManager, UPDATE_SQUIBS, true, false, NEVER);
        for(uint8_t ch = 0; ch < NUM_EM_CHANNELS; ch++) {
            if (slate[ch].arm() && slate[ch].state() == FIRED) {
                squibChannel_t firedsquib = ch_map[ch];
                (firedsquib.squib)->fire(firedsquib.firecmd);
            }
        }
    }
}

bool FireTask::fireEmatch(uint8_t ch, bool update){
    if(ch >= 0 && ch < NUM_EM_CHANNELS){
        slate[ch].state << FIRED;
        if (update) {
            _updateSquibs();
        }
        return true;
    } else { return false; }
}

void FireTask::_updateSquibs() {
    xEventGroupSetBits(squibManager, UPDATE_SQUIBS);
}