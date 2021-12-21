#include "StateData.hpp"
#include "main.hpp"

StaticJsonDocument<1024> StateData::stateJSON;
Mutex StateData::SVmutex; // mutex for solenoid valve state
Mutex StateData::EMmutex; // mutex for ematch state
Mutex StateData::senseMutex; // mutex for sensor state
Mutex StateData::seqMutex; // mutex for sequence state/name
Mutex StateData::errMutex; // mutex for error

StaticJsonDocument<1024>* StateData::getState()
{
    stateJSON.clear(); // clear memory pool of JSON
    // Update time
    stateJSON["tick"] = xTaskGetTickCount();
    // Update statepacket from sensor, solenoid, and ematch statuses
    for(uint8_t i = 0; i < Sensor::num_sensors; i++)
        stateJSON["sense"][(sys.sensors[i])->ch_name] = sys.statedata.getSensorState(i);
    stateJSON["SV"] = sys.statedata.getSolenoidState(); // most efficient way to send is just as the raw uint, can decode on groundside
    stateJSON["EM"] = sys.statedata.getEmatchState(); // most efficient way to send is just as the raw uint, can decode on groundside
    stateJSON["logging"] = sys.tasks.logger.isLoggingEnabled();
    char* seq = sys.statedata.getSequence();
    if(strlen(seq)){ // if a sequence is loaded
        stateJSON["seq"]["name"] = sys.statedata.getSequence();
        stateJSON["seq"]["start"] = sys.tasks.sequencetask.isSequenceStarted();
        stateJSON["seq"]["play"] = sys.tasks.sequencetask.isSequencePlaying();
    }
    char* err = sys.statedata.getError();
    if(strlen(err)) {// if an error is present
        stateJSON["error"] = err;
    }
    return &stateJSON;
};

void StateData::setSensorState(uint8_t ch_id, float ch_val)
{
    senseMutex.take(NEVER);
    sensorstate[ch_id] = ch_val;
    senseMutex.give();
};

void StateData::setSolenoidState(uint8_t sol_ch, solenoid_state_t solenoid_state,bool updateSVs)
{
    SVmutex.take(NEVER);
    // clear then set the bit corresponding to the desired sol_ch (zero indexed)
    solenoidstate = (solenoidstate & ~(1U << sol_ch)) | (solenoid_state << sol_ch); 
    SVmutex.give();
    if(updateSVs)
        sys.tasks.valvetask.updateValves(); // send signal to valvetask that solenoid state has changed
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

float StateData::getSensorState(uint8_t ch_id)
{
    senseMutex.take(NEVER);
    float temp = sensorstate[ch_id];
    senseMutex.give();
    return temp;
};

uint8_t StateData::getSolenoidState()
{
    SVmutex.take(NEVER);
    uint8_t temp = solenoidstate;
    SVmutex.give();
    return temp;
};

uint8_t StateData::getEmatchState()
{
    EMmutex.take(NEVER);
    uint8_t temp = ematchstate;
    EMmutex.give();
    return temp;
};

void StateData::setSequence(const char* seq_name){
    seqMutex.take(NEVER);
    memcpy(currSequence, seq_name, MAX_TEST_FILENAME_SIZE);
    currSequence[MAX_TEST_FILENAME_SIZE] = '\0'; //ensure null-terminated
    seqMutex.give();
};

char* StateData::getSequence(){
    return currSequence;
};

void StateData::clearSequence(){
    seqMutex.take(NEVER);
    currSequence[0] = '\0'; // empty name means no sequence
    seqMutex.give();
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