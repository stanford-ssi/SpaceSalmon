#include "LoggerTask.hpp"

TaskHandle_t LoggerTask::taskHandle = NULL;
StaticTask_t LoggerTask::xTaskBuffer;
StackType_t LoggerTask::xStack[stackSize];

MessageBufferHandle_t LoggerTask::bufferHandle;
StaticMessageBuffer_t LoggerTask::messageBufferStruct;
uint8_t LoggerTask::ucStorageBuffer[bufferSize];

char LoggerTask::lineBuffer[bufferSize];

bool LoggerTask::loggingEnabled = false;

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

void LoggerTask::log(char *message)
{
    vPortEnterCritical();
    xMessageBufferSend(bufferHandle, message, strlen(message) + 1, 0);
    vPortExitCritical();
}

void LoggerTask::format()
{
    FRESULT res;
    printf("-I- Format disk %d\n\r", 0);
    printf("-I- Please wait a moment during formatting...\n\r");
    //res = f_mkfs("0", FM_EXFAT, 512);
    printf("-I- Disk format finished !\n\r");
    if (res != FR_OK)
    {
        printf("-E- f_mkfs pb: 0x%X\n\r", res);
    }
}

void LoggerTask::activity(void *ptr)
{
    gpio_set_pin_level(DISK_LED, true);
    FRESULT res;

    //Clear file system object
    memset(&fs, 0, sizeof(FATFS));

    res = f_mount(&fs, "", 1);
    if (res != FR_OK)
    {
        loggingEnabled = false;
        printf("WARN-%s-%u: 0x%X\n\r", __FILE__, __LINE__, res);
    }
    else
    {
        loggingEnabled = true;
        printf("Mounted SD card, enabling logging\n");
    }

    char file_name[20];

    if (loggingEnabled)
    {
        int lognum = 0;
        while (true)
        {
            snprintf(file_name, sizeof(file_name), "log%u.txt", lognum);
            res = f_stat(file_name, NULL);

            if (res == FR_NO_FILE)
            {
                break;
            }

            if (res != FR_OK)
            {
                printf("WARN-%s-%u: 0x%X\n\r", __FILE__, __LINE__, res);
                printf("->Error stat-ing file: %s\n", file_name);
                loggingEnabled = false;
                break;
            }

            lognum++;
        }
    }

    if (loggingEnabled)
    {
        printf("Logging to file: %s\n", file_name);

        res = f_open(&file_object, file_name, FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
        if (res != FR_OK)
        {
            printf("WARN-%s-%u: 0x%X\n\r", __FILE__, __LINE__, res);
            loggingEnabled = false;
        }
    }
    gpio_set_pin_level(DISK_LED, false);

    while (true)
    {
        if (xMessageBufferReceive(bufferHandle, lineBuffer, sizeof(lineBuffer), portMAX_DELAY) > 0)
        {
            printf_("%s\n", lineBuffer);
            
            uint8_t unsaved = 0;

            if (loggingEnabled)
            {
                gpio_set_pin_level(DISK_LED, true);

                uint32_t len = strlen(lineBuffer);
                if (len < sizeof(lineBuffer) - 1)
                {
                    lineBuffer[len] = '\n';
                    lineBuffer[len + 1] = '\0';
                }
                else
                {
                    lineBuffer[sizeof(lineBuffer) - 2] = '\n';
                    lineBuffer[sizeof(lineBuffer) - 1] = '\0';
                }

                res = (FRESULT)f_puts(lineBuffer, &file_object);
                if (res < 0)
                {
                    printf("WARN-%s-%u: 0x%X\n\r", __FILE__, __LINE__, res);
                }

                if(unsaved > 10){
                    res = f_sync(&file_object); //the file is still saved every for each sector, which is pretty fast... (false!)
                    if (res != FR_OK)
                    {
                        printf("WARN-%s-%u: 0x%X\n\r", __FILE__, __LINE__, res);
                    }
                }else{
                    unsaved++;
                }
                
                gpio_set_pin_level(DISK_LED, false);
            }
        }
    }
}