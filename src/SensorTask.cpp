#include "SensorTask.hpp"

TaskHandle_t SensorTask::taskHandle = NULL;
StaticTask_t SensorTask::xTaskBuffer;
StackType_t SensorTask::xStack[stackSize];

SensorTask::SensorTask()
{

    //assert(xTaskCreate(activity, "Sensors", 1000, NULL, TASK_EXAMPLE_STACK_PRIORITY, &taskHandle) == pdPASS,__FILE__,__LINE__);
    SensorTask::taskHandle = xTaskCreateStatic(activity,//static function to run
                                "Sensors",  //task name
                                stackSize,  //stack depth (words!)
                                NULL,       //parameters
                                1,          //priority
                                SensorTask::xStack,     //stack object
                                &SensorTask::xTaskBuffer);//TCB object*/
}

TaskHandle_t SensorTask::getTaskHandle(){
    return taskHandle;
}

void SensorTask::activity(void * ptr){
    
    ADXL375 adxl375(&SPI_SENSOR, ADXL_CS_1);
    adxl375.init();
    adxl375.startMeasuring();

    BMP3xx bmp388(&SPI_SENSOR, BMP_CS_1);
    bmp388.begin();

    BMI088Accel bmi088accel(&SPI_SENSOR,ACCEL_CS_1);
	bmi088accel.begin();

	BMI088Gyro bmi088gyro(&SPI_SENSOR,GYRO_CS_1);
    bmi088gyro.begin();

    ADXL375::Data accelHigh;
    BMP3xx::Data pressure;
    BMI088Gyro::Data gyro;
    BMI088Accel::Data accel;


    while(true){
        vTaskDelay(100);

        //assert(uxTaskGetStackHighWaterMark(NULL) > 10,"Out of Stack!",1);   

        accelHigh = adxl375.readSensor();
        pressure = bmp388.readSensor();
        accel = bmi088accel.readSensor();
        gyro = bmi088gyro.readSensor();

        printf_(" High:{%f %f %f} ", accelHigh.x, accelHigh.y, accelHigh.z);
        printf_(" Pres:{%f %f} ", pressure.pressure, pressure.temperature);
        printf_(" Gyro:{%f %f %f} ", gyro.x, gyro.y, gyro.z);
        printf_(" Accel:{%f %f %f %lu} \n", accel.x, accel.y, accel.z, accel.time);
        
    }
}