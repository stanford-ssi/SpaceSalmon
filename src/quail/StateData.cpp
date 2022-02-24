#include "StateData.hpp"
#include "main.hpp"

StaticJsonDocument<1024> StateData::stateJSON;
Mutex StateData::EMmutex; // mutex for ematch state
Mutex StateData::errMutex; // mutex for error

StaticJsonDocument<1024>* StateData::getState()
{
    stateJSON.clear(); // clear memory pool of JSON
    // Update time
    stateJSON["tick"] = xTaskGetTickCount();
    stateJSON["EM"] = sys.statedata.getEmatchState(); // most efficient way to send is just as the raw uint, can decode on groundside
    stateJSON["logging"] = sys.tasks.logger.isLoggingEnabled();
    char* err = sys.statedata.getError();
    if(strlen(err)) {// if an error is present
        stateJSON["error"] = err;
    }
    return &stateJSON;
};


void StateData::fireEmatch(uint8_t ematch_ch, bool updateEMs)
{
    EMmutex.take(NEVER);
    // clear then set the bit corresponding to the desired sol_ch (zero indexed)
    ematchstate = (ematchstate | (FIRED << ematch_ch)); // set channelto indicate fired status
    EMmutex.give();
    if(updateEMs)
        sys.tasks.firetask.fireEmatch(ematch_ch); // send signal to firetask that ematch state has changed
};


uint8_t StateData::getEmatchState()
{
    EMmutex.take(NEVER);
    uint8_t temp = ematchstate;
    EMmutex.give();
    return temp;
};


void StateData::setError(const char* error_msg){
    errMutex.take(NEVER);
    memcpy(error_buf, error_msg, strlen(error_msg));
    errMutex.give();
};

char* StateData::getError(){
    return error_buf;
}

void StateData::clearError(){
    errMutex.take(NEVER);
    error_buf[0] = '\0'; // clear error
    errMutex.give();
}