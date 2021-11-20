#include "FireTask.hpp"

FireTask::FireTask(uint8_t priority, uint8_t Squib1_SS, uint8_t Squib2_SS) : Task(priority, "Fire"), squib1(sys.squib_spi,Squib1_SS), squib2(sys.squib_spi,Squib2_SS)
{
    evgroup = xEventGroupCreateStatic(&evbuf);
    // TODO: make this less gross? and maybe settable at System level
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
    squib2.Init();
    while(true) {
        uint32_t flags = xEventGroupWaitBits(evgroup, UINT32_MAX, true, false, NEVER); // any flag should be inspected
        for(uint8_t i = 0; i < NUM_EM_CHANNELS; i++) {
            if((flags & 0b1)) { // if the lsb bit is 1
                squibChannel_t firedsquib = ch_map[i];
                (firedsquib.squib)->fire(firedsquib.firecmd);
            }
            flags = flags >> 1; // shift right to check next lsb
        }
    }
}

void FireTask::fireEmatch(uint8_t ch){
    xEventGroupSetBits(evgroup, ch); // set event group flag to be the channel to fire
}