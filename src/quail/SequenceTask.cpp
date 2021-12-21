#include "SequenceTask.hpp"
#include "RXTask.hpp"
#include "main.hpp"

StaticEventGroup_t SequenceTask::evBuf;
EventGroupHandle_t SequenceTask::evGroup;
TimerHandle_t SequenceTask::delayTimer;
StaticTimer_t SequenceTask::delaybuf; 

SequenceTask::SequenceTask(uint8_t priority):Task(priority, "Sequence"){
    evGroup = xEventGroupCreateStatic(&evBuf);
    delayTimer = xTimerCreateStatic("seqdelay",10000,pdFALSE,(void*)0, SequenceTask::_play_seq, &delaybuf);
}

void SequenceTask::activity()
{
    vTaskDelay(7000);

    while(true){
        if(!seqbuf.empty()){ // check to see if we should attempt to load a new sequence
            _load_seq();
            if(!filebuf.empty()){ // if we succesfully loaded a new sequence, update the file object
                filebuf.receive(seq_file_object, true);
        }
        }
        xEventGroupWaitBits(evGroup, SEQ_LOADED & SEQ_STARTED & SEQ_LOADED, pdFALSE, pdTRUE, NEVER); // wait for a started, loaded, playing sequence
        if(!f_eof(&seq_file_object)){
            f_gets(seqLineBuffer, sizeof(seqLineBuffer), &seq_file_object);
            sys.tasks.rxtask.sendcmd(seqLineBuffer); // send this line to rxtask for processing as a Quail command
        }
        else{
            stop_seq(); // got to end of sequence file! stop the sequence
        }
    };
};

void SequenceTask::start_seq(){
    xEventGroupSetBits(evGroup, SEQ_STARTED);
};

void SequenceTask::stop_seq(){
    xEventGroupClearBits(evGroup, SEQ_STARTED);
};

void SequenceTask::pause_seq(){
    xEventGroupClearBits(evGroup, SEQ_PLAYING);
};
void SequenceTask::play_seq(){
    xEventGroupSetBits(evGroup, SEQ_PLAYING);
};

void SequenceTask::_play_seq(TimerHandle_t xTimer){
    xTimerStop(delayTimer, NEVER);
    xEventGroupSetBits(evGroup, SEQ_PLAYING);
};

void SequenceTask::load_seq(const char* seq_name){
    char seq_name_arr[MAX_TEST_FILENAME_SIZE];
    memcpy(seq_name_arr, seq_name, MAX_TEST_FILENAME_SIZE); // copy data over
    seqbuf.send(seq_name_arr); // send to buffer
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