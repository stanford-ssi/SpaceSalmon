#include "LoggerTask.hpp"

TaskHandle_t LoggerTask::taskHandle = NULL;
StaticTask_t LoggerTask::xTaskBuffer;
StackType_t LoggerTask::xStack[stackSize];

MessageBufferHandle_t LoggerTask::bufferHandle;
StaticMessageBuffer_t LoggerTask::messageBufferStruct;
uint8_t LoggerTask::ucStorageBuffer[bufferSize];

char LoggerTask::lineBuffer[bufferSize];

FATFS LoggerTask::fs;
FIL LoggerTask::file_object;

LoggerTask::LoggerTask()
{
    LoggerTask::taskHandle = xTaskCreateStatic(activity,                  //static function to run
                                               "Logger",                  //task name
                                               stackSize,                 //stack depth (words!)
                                               NULL,                      //parameters
                                               1,                         //priority
                                               LoggerTask::xStack,        //stack object
                                               &LoggerTask::xTaskBuffer); //TCB object
    
    LoggerTask::bufferHandle = xMessageBufferCreateStatic(bufferSize, LoggerTask::ucStorageBuffer, &LoggerTask::messageBufferStruct);
}

TaskHandle_t LoggerTask::getTaskHandle()
{
    return taskHandle;
}

void LoggerTask::log(char* message){
    vPortEnterCritical();
    xMessageBufferSend(bufferHandle,message,strlen(message)+1,0);
    vPortExitCritical();
}

void LoggerTask::activity(void *ptr)
{
    FRESULT res;

    //Mount disk
	printf("-I- Mount disk %d\n\r", 0);
	//Clear file system object
	memset(&fs, 0, sizeof(FATFS));
	res = f_mount(&fs, "", 1);
	if (res != FR_OK)
	{
		printf("-E- f_mount pb: 0x%X\n\r", res);
	}

	printf("-I- Create a file : \"%s\"\n\r", "0:/log.txt");
	res = f_open(&file_object, "0:/log.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);//todo, give it a name!
	if (res != FR_OK)
	{
		printf("-E- f_open create pb: 0x%X\n\r", res);
	}

    while(true){
        if(xMessageBufferReceive(bufferHandle, lineBuffer, sizeof(lineBuffer), portMAX_DELAY) > 0){
            printf_("LOG: %s\n",lineBuffer);

            uint32_t len = strlen(lineBuffer);
            if(len < sizeof(lineBuffer)-1){
                lineBuffer[len] = '\n';
                lineBuffer[len+1] = '\0';
            }else{
                lineBuffer[sizeof(lineBuffer)-2] = '\n';
                lineBuffer[sizeof(lineBuffer)-1] = '\0';
            }

            res = (FRESULT)f_puts(lineBuffer, &file_object);
            if (res < 0)
            {
                printf("-E- f_puts pb: 0x%X\n\r", res);
            }

            res = f_sync(&file_object);
            if (res != FR_OK)
            {
                printf("-E- f_sync pb: 0x%X\n\r", res);
            }

            printf("Saved!\n");
        }
    }
    
}