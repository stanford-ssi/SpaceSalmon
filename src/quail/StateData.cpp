#include "StateData.hpp"
#include "main.hpp"

StaticJsonDocument<1024> StateData::stateJSON;
Mutex StateData::EMmutex; // mutex for ematch state
Mutex StateData::errMutex; // mutex for error

StaticJsonDocument<1024>* StateData::getState()
{
    stateJSON.clear(); // clear memory pool of JSON
    // Update time
    stateJSON["logging"] = sys.tasks.logger.isLoggingEnabled();
    char* err = sys.statedata.getError();
    if(strlen(err)) {// if an error is present
        stateJSON["error"] = err;
    }
    return &stateJSON;
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