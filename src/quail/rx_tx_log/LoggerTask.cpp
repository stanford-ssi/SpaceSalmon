#include "LoggerTask.hpp"
#include "main.hpp"

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
            sys.statedata.setError("A log file is corrupted");
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
        sys.statedata.setError("Could Not Mount Disk");
        memset(&fs, 0, sizeof(FATFS));
        res = f_mount(&fs, "", 1);
        vTaskDelay(5000);
    } 

    char file_name[20];
    int file_num = 0;
    do {
        findFile(file_name, 20, &file_num);
        sys.statedata.setError("Trying to open log file");
        res = f_open(&file_object, file_name, FA_CREATE_ALWAYS | FA_WRITE);

    } while(res != FR_OK);
    digitalWrite(DISK_LED, true); // led comes on when we're sure we can write to files

    // Writing to SD is time consuming so we're going to keep everyting in a buffer and
    // increment the values until we really need to dump it (either 10000 characters)
    // read or more than 1000ms elapsed since last dump)
    char* p = lineBuffer; // start at beginning of buffer
    TickType_t timeout = xTaskGetTickCount();
    while (loggingEnabled) {
        if (strBuffer.receive(p, 1000, !shitlEnabled) > 0) { // adds data onto buffer, blocking
            // denote end of one message with new line
            p = lineBuffer + strlen(lineBuffer);
            p[0] = '\n';
            p++;
            p[0] = '\0';

            // is the buffer full? have we waited too long? then we need tow rite
            if (p - lineBuffer > 8999 || xTaskGetTickCount() > timeout) { 
                writeSD(lineBuffer);

                lineBuffer[0] = '\0';
                p = lineBuffer;
                timeout = xTaskGetTickCount() + 1000; //if there are no logs for a bit, we should still flush every once and a while
            }
        }
        else { //if timeout is NEVER, we don't ever reach this (no SHITL)
            readSHITL();
        }
    }
}

void LoggerTask::readSHITL() {
    // digitalWrite(SENSOR_LED, true);
    if (!f_eof(&shitl_file_object))
    {
        f_gets(inputLineBuffer, sizeof(inputLineBuffer), &shitl_file_object);
        StaticJsonDocument<1024> sensor_json;

        if (deserializeJson(sensor_json, inputLineBuffer) == DeserializationError::Ok) {
            // there used to be guppy stuff here
        }
        else {
            printf("Parsing Error!\n");
        }
    }
}

void LoggerTask::writeSD(char *buf) {
    digitalWrite(DISK_LED, true);

    FRESULT res;
    UINT writen;
    res = f_write(&file_object, buf, strlen(buf), &writen);

    if (res != FR_OK) {
        loggingEnabled = false;
        digitalWrite(DISK_LED, false);
        sys.tasks.logger.log("SD Write Error");
    }

    res = f_sync(&file_object); //update file structure
    if (res != FR_OK) {
        loggingEnabled = false;
        digitalWrite(DISK_LED, false);
        sys.tasks.logger.log("SD Flush Error");
    }
}