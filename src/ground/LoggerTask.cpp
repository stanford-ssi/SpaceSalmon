#include "LoggerTask.hpp"
#include "main.hpp"

LoggerTask::LoggerTask(uint8_t priority) : Task(priority, "Logger") {}

void LoggerTask::log(const char *message)
{
    logBuffer.send(message, strlen(message) + 1);
}

void LoggerTask::logJSON(JsonDocument &jsonDoc, const char *id)
{
    jsonDoc["id"] = id;
    jsonDoc["stack"] = uxTaskGetStackHighWaterMark(NULL); //TODO: Check this for capacity... (dangerous!)

    if (jsonDoc.getMember("tick") == NULL)
    {
        jsonDoc["tick"] = xTaskGetTickCount();
    }

    //jsonDoc["la"] = xMessageBufferSpaceAvailable(bufferHandle);

    size_t len = measureJson(jsonDoc);
    char str[len + 5]; //plenty of room!
    serializeJson(jsonDoc, str, sizeof(str));
    log(str);
}

void LoggerTask::log(JsonDocument &jsonDoc)
{
    jsonDoc["stack"] = uxTaskGetStackHighWaterMark(NULL); //TODO: Check this for capacity... (dangerous!)

    if (jsonDoc.getMember("tick") == NULL)
    {
        jsonDoc["tick"] = xTaskGetTickCount();
    }

    size_t len = measureJson(jsonDoc);
    char str[len + 5]; //plenty of room!
    serializeJson(jsonDoc, str, sizeof(str));
    log(str);
}

void LoggerTask::activity(void *ptr)
{

    Serial.begin(9600);

    char *p = logLineBuffer;
    char *q = inputLineBuffer;
    TickType_t timeout = 0;
    while (true)
    {
        //Step 1: read in all the logs
        if (logBuffer.receiveTimeout(logLineBuffer, 1000, 500) > 0)
        {

            Serial.println(logLineBuffer);

            //p = logLineBuffer + strlen(logLineBuffer);

            // p[0] = '\n';
            // p++;
            // p[0] = '\0';

            //if (p - logLineBuffer > 8999 || xTaskGetTickCount() > timeout)
            //{ //we need to write!
                //Step 2: Write to USB
                //Serial.print(logLineBuffer);

                //reset buffer
            //    logLineBuffer[0] = '\0';
            //    p = logLineBuffer;
            //    timeout = xTaskGetTickCount() + 1000; //if there are no logs for a bit, we should still flush every once and a while
            //}
        }

        int count = Serial.available();
        while (count > 0)
        {
            Serial.readBytes(q, count);
            q += count;
            if (*(q - 1) == '\n')
            {
                if (*(q - 2) == '\r')
                    q--;
                q--;
                *(q) = '\0';
                inputBuffer.send(inputLineBuffer, strlen(inputLineBuffer));
                q = inputLineBuffer;
            }
            count = Serial.available();
        }
    }
}