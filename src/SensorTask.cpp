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

    //ADXL375 adxl375(&SPI_SENSOR, ADXL_CS_1);
    //adxl375.init();
    //adxl375.startMeasuring();

    BMP3xx bmp388(&SPI_SENSOR, BMP_CS_1);
    bmp388.begin();

    //BMI088Accel bmi088accel(&SPI_SENSOR, ACCEL_CS_1);
    //bmi088accel.begin();

    BMI088Gyro bmi088gyro(&SPI_SENSOR, GYRO_CS_1);
    bmi088gyro.begin();

    ADXL375::Data accelHigh;
    BMP3xx::Data pressure;
    BMI088Gyro::Data gyro;
    BMI088Accel::Data accel;

    while (true)
    {
        vTaskDelay(100);

        assert(uxTaskGetStackHighWaterMark(NULL) > 10,"Out of Stack!",1);

        //accelHigh = adxl375.readSensor();
        //vTaskDelay(100);
        pressure = bmp388.readSensor();
        vTaskDelay(10);
        //accel = bmi088accel.readSensor();
        //vTaskDelay(100);
        gyro = bmi088gyro.readSensor();
        //vTaskDelay(100);

        printf_(" High:{%f %f %f} ", accelHigh.x, accelHigh.y, accelHigh.z);
        printf_(" Pres:{%f %f} ", pressure.pressure, pressure.temperature);
        printf_(" Gyro:{%f %f %f} ", gyro.x, gyro.y, gyro.z);
        printf_(" Accel:{%f %f %f %lu} ", accel.x, accel.y, accel.z, accel.time);
        printf_(" Stack: %u \n", uxTaskGetStackHighWaterMark(NULL));
    }
}