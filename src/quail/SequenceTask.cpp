#include "SequenceTask.hpp"
#include "main.hpp"

TimerHandle_t SequenceTask::delayTimer;
StaticTimer_t SequenceTask::delaybuf; 
SequenceTask* SequenceTask::glob_ptr;

SequenceTask::SequenceTask(uint8_t priority):Task(priority, "Sequence"){
    evGroup = xEventGroupCreateStatic(&evBuf);
    delayTimer = xTimerCreateStatic("seqdelay",10000,pdFALSE,(void*)0, SequenceTask::_play_seq, &delaybuf);
    glob_ptr = this;
}

void SequenceTask::activity()
{
    while(true){
        // Block until a new file name is ready to be loaded
        _load_seq(); 
        if(!filebuf.empty()){ // if we succesfully loaded a new sequence, update the file object
            filebuf.receive(seq_file_object, true);
        }
        // Check the sequence status
        uint32_t res = xEventGroupWaitBits(evGroup, SEQ_LOADED | SEQ_STARTED | SEQ_PLAYING, pdFALSE, pdTRUE, 1); // wait for a started, loaded, playing sequence
        // While a sequence is loaded
        while(res & SEQ_LOADED){
            // If started and playing
            if((res & SEQ_STARTED) && (res & SEQ_PLAYING)) {
                // if not at end of file, read the next file line and process it likea command
                if(!f_eof(&seq_file_object)){
                    f_gets(seqLineBuffer, sizeof(seqLineBuffer), &seq_file_object);
                    sys.tasks.rxtask.sendcmd(seqLineBuffer); // send this line to rxtask for processing as a Quail command
                }
                // if at the end of the file
                else{
                    stop_seq(); // got to end of sequence file! stop the sequence
                }
            }
            // update group flags
            res = xEventGroupWaitBits(evGroup, SEQ_LOADED | SEQ_STARTED | SEQ_PLAYING, pdFALSE, pdTRUE, 1);
        }
    };
};

void SequenceTask::start_seq(){
    started = true;
    if(!playing) // if we were paused, restart the sequence
        load_seq(sys.statedata.getSequence()); // this re-loads the current sequence
    xEventGroupSetBits(evGroup, SEQ_STARTED); // start the sequence
};

void SequenceTask::stop_seq(){
    pause_seq(); // set to paused so that if start is called, the sequence is re-loaded
    started = false; 
    xEventGroupClearBits(evGroup, SEQ_STARTED);
};

void SequenceTask::pause_seq(){
    if(started){ // can only pause a started sequence
        playing = false;
        xEventGroupClearBits(evGroup, SEQ_PLAYING);
    }
};
void SequenceTask::play_seq(){
    if(started){ // can only play a started sequence (if already playing, this does nothing)
        playing = true;
        xEventGroupSetBits(evGroup, SEQ_PLAYING);
    }
};

void SequenceTask::_play_seq(TimerHandle_t xTimer){
    xTimerStop(delayTimer, NEVER);
    xEventGroupSetBits(glob_ptr->evGroup, SEQ_PLAYING);
};

void SequenceTask::load_seq(const char* seq_name){
    char seq_name_arr[MAX_TEST_FILENAME_SIZE];
    memcpy(seq_name_arr, seq_name, MAX_TEST_FILENAME_SIZE); // copy data over
    seqbuf.send(seq_name_arr); // send to buffer
    xEventGroupClearBits(evGroup, SEQ_LOADED); // sequence is not loaded, needs to be!
};

void SequenceTask::_load_seq(){
    char seq_name[MAX_TEST_FILENAME_SIZE];
    seqbuf.receive(seq_name, true); // load the filename from the msg buf
    char seq_filename[MAX_TEST_FILENAME_SIZE+5];
    snprintf(seq_filename, sizeof(seq_filename), "%s.txt", seq_name); // get full filename
    FRESULT res = f_stat(seq_filename, NULL);
    FIL new_file_object;
    if( res == FR_OK){
        res = f_open(&new_file_object, seq_filename, FA_READ); // open file for reading
        if( res == FR_OK ){
            sys.statedata.setSequence(seq_name); // set sequence name in StateData
            filebuf.send(new_file_object); // send the new file object
            xEventGroupSetBits(evGroup, SEQ_LOADED); // indicate successful sequence load
            return;
        }
    }
    xEventGroupClearBits(evGroup, SEQ_LOADED);
};

void SequenceTask::delay_seq(uint16_t ticks_to_wait){
    xTimerChangePeriod(delayTimer, ticks_to_wait, NEVER);
    pause_seq();
    xTimerStart(delayTimer, NEVER);
};