#include "SensorTask.hpp"

TaskHandle_t SensorTask::taskHandle = NULL;
StaticTask_t SensorTask::xTaskBuffer;
StackType_t SensorTask::xStack[stackSize];
StaticSemaphore_t SensorTask::mutexBuffer[4];

SensorTask::SensorTask()
{
    SensorTask::taskHandle = xTaskCreateStatic(activity,                  //static function to run
                                               "Sensors",                 //task name
                                               stackSize,                 //stack depth (words!)
                                               NULL,                      //parameters
                                               1,                         //priority
                                               SensorTask::xStack,        //stack object
                                               &SensorTask::xTaskBuffer); //TCB object

    adxl375_mutex = xSemaphoreCreateMutexStatic(&SensorTask::mutexBuffer[0]);
    bmp388_mutex = xSemaphoreCreateMutexStatic(&SensorTask::mutexBuffer[1]);
    bmi088accel_mutex = xSemaphoreCreateMutexStatic(&SensorTask::mutexBuffer[2]);
    bmi088gyro_mutex = xSemaphoreCreateMutexStatic(&SensorTask::mutexBuffer[3]);
}

TaskHandle_t SensorTask::getTaskHandle()
{
    return taskHandle;
}

void SensorTask::activity(void *ptr)
{

    ADXL375 adxl375(&SPI_SENSOR, ADXL_CS_1);
    adxl375.init();
    adxl375.startMeasuring();

    BMP3xx bmp388(&SPI_SENSOR, BMP_CS_1);
    bmp388.begin();

    BMI088Accel bmi088accel(&SPI_SENSOR, ACCEL_CS_1);
    bmi088accel.begin();

    BMI088Gyro bmi088gyro(&SPI_SENSOR, GYRO_CS_1);
    bmi088gyro.begin();

    ADXL375::Data accelHigh;
    BMP3xx::Data pressure;
    BMI088Gyro::Data gyro;
    BMI088Accel::Data accel;

    TickType_t lastSensorTime = xTaskGetTickCount();

    while (true)
    {
        vTaskDelayUntil(&lastSensorTime, 20);

        gpio_set_pin_level(SENSOR_LED,true);

        assert(uxTaskGetStackHighWaterMark(NULL) > 10,"Out of Stack!",1);

        StaticJsonDocument<1000> sensor_json;

        accelHigh = adxl375.readSensor();
        pressure = bmp388.readSensor();
        vTaskDelay(1);//but why...
        accel = bmi088accel.readSensor();
        vTaskDelay(1);//but why...
        gyro = bmi088gyro.readSensor();

        sensor_json["tick"] = xTaskGetTickCount();

        JsonObject bmp388_json = sensor_json.createNestedObject("bmp");
        JsonObject bmi088_json = sensor_json.createNestedObject("bmi");
        JsonObject adxl375_json = sensor_json.createNestedObject("adxl");

        bmp388_json["p"] = pressure.pressure;
        bmp388_json["t"] = pressure.temperature;
        
        JsonArray bmi_accel_json = bmi088_json.createNestedArray("a");
        bmi_accel_json.add(accel.x);
        bmi_accel_json.add(accel.y);
        bmi_accel_json.add(accel.z);

        JsonArray bmi_gyro_json = bmi088_json.createNestedArray("g");
        bmi_gyro_json.add(gyro.x);
        bmi_gyro_json.add(gyro.y);
        bmi_gyro_json.add(gyro.z);

        JsonArray adxl_accel_json = adxl375_json.createNestedArray("a");
        adxl_accel_json.add(accelHigh.x);
        adxl_accel_json.add(accelHigh.y);
        adxl_accel_json.add(accelHigh.z);
        
        Globals::logger.logJSON(sensor_json,"sensor");

        gpio_set_pin_level(SENSOR_LED,false);
    }
}