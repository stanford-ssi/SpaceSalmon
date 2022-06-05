#include "LoggerTask.hpp"
#include "main.hpp"
#include "../config.h"

LoggerTask::LoggerTask(uint8_t priority) : Task(priority, "Logger") {
}

void LoggerTask::log(const char* message) {
    strBuffer.send(message, strlen(message) + 1);
}

void LoggerTask::log(JsonDocument& jsonDoc) {
    jsonDoc["stack"] = uxTaskGetStackHighWaterMark(NULL); //TODO: Check this for capacity... (dangerous!)

    if (jsonDoc.getMember("tick") == NULL) {
        jsonDoc["tick"] = xTaskGetTickCount();
    }

    size_t len = measureJson(jsonDoc);
    char str[len + 5]; //plenty of room!
    serializeJson(jsonDoc, str, sizeof(str));
    log(str);
}

void LoggerTask::logJSON(JsonDocument& jsonDoc, const char* id) {
    jsonDoc["id"] = id;
    log(jsonDoc);
}

void LoggerTask::findFile(char* filename, size_t filesize, int* lognum) {
    for(FRESULT res = FR_OK; res != FR_NO_FILE; (*lognum)++) {
        snprintf(filename, filesize, "log%u.txt", *lognum);
        res = f_stat(filename, NULL);
        if (res != FR_OK) {
            sys.slate.board.error << ErrorType::corrupt_file;
        }
    }
}

void LoggerTask::activity()
{
    SSISD sd;
    sd.init();
    vTaskDelay(6000); 

    // Clear file system object
    memset(&fs, 0, sizeof(FATFS));
    FRESULT res = f_mount(&fs, "", 1);

    while (res != FR_OK) {
        loggingEnabled = false;
        sys.slate.board.error << ErrorType::failed_to_mount_disk;
        memset(&fs, 0, sizeof(FATFS));
        res = f_mount(&fs, "", 1);
        vTaskDelay(5000);
    } 

    char file_name[20];
    int file_num = 0;
    do {
        findFile(file_name, 20, &file_num);
        sys.slate.board.error << ErrorType::failed_to_open_file;
        res = f_open(&file_object, file_name, FA_CREATE_ALWAYS | FA_WRITE);

    } while(res != FR_OK);
    digitalWrite(LOG_LED, true); // led comes on when we're sure we can write to files

    // Writing to SD is time consuming so we're going to keep everyting in a buffer and
    // increment the values until we really need to dump it (either 10000 characters)
    // read or more than 1000ms elapsed since last dump)
    char* p = lineBuffer; // start at beginning of buffer
    TickType_t timeout = xTaskGetTickCount();
    while (loggingEnabled) {
        if (strBuffer.receive(p, (lineBuffer + sizeof(lineBuffer) - p), !shitlEnabled) > 0) { // adds data onto buffer, blocking. Available size is calculate based on size of.
            // denote end of one message with new line
            p = lineBuffer + strlen(lineBuffer);
            p[0] = '\n';
            p++;
            p[0] = '\0';

            // is the buffer full? have we waited too long? then we need to write
            if (p - lineBuffer > 8999 || xTaskGetTickCount() > timeout) { 
                writeSD(lineBuffer);

                lineBuffer[0] = '\0';
                p = lineBuffer;
                timeout = xTaskGetTickCount() + 1000; //if there are no logs for a bit, we should still flush every once and a while
            }
        }
    }
}

void LoggerTask::writeSD(char *buf) {
    digitalWrite(LOG_LED, true);

    FRESULT res;
    UINT writen;
    res = f_write(&file_object, buf, strlen(buf), &writen);

    if (res != FR_OK) {
        loggingEnabled = false;
        digitalWrite(LOG_LED, false);
        this->log("SD Write Error");
    }

    res = f_sync(&file_object); //update file structure
    if (res != FR_OK) {
        loggingEnabled = false;
        digitalWrite(LOG_LED, false);
        this->log("SD Flush Error");
    }
}