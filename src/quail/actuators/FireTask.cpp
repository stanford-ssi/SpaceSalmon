#include "FireTask.hpp"
#include "main.hpp"

FireTask::FireTask(uint8_t priority, uint8_t Squib_SS) : 
Task(priority, "Fire"), squib(sys.squib_spi, Squib_SS), slate(sys.slate.squib)
{
    squibManager = xEventGroupCreateStatic(&evbuf);

    ch_map[0] = {&squib, CMD_FIRE_1B};
    ch_map[1] = {&squib, CMD_FIRE_2B};
    ch_map[2] = {&squib, CMD_FIRE_2A};
    ch_map[3] = {&squib, CMD_FIRE_1A};
};

void FireTask::activity()
{
    sys.squib_spi.begin();
    squib.Init();
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