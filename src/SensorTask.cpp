#include "SensorTask.hpp"

SensorTask::SensorTask()
{
    //TODO: xTaskCreateStatic()
    if (xTaskCreate(sensorActivity, "Sensors", 1000, NULL, TASK_EXAMPLE_STACK_PRIORITY, &taskHandle) != pdPASS) {
        printf_("Sensor Task Creation Failed!");
	}
    
}

TaskHandle_t SensorTask::getTaskHandle(){
    return taskHandle;
}

void SensorTask::sensorActivity(void * ptr){

    bool rc;
    
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

        assert(uxTaskGetStackHighWaterMark(NULL),"kek",1);   

         accelHigh = adxl375.readSensor();
         pressure = bmp388.readSensor();
         accel = bmi088accel.readSensor();
         gyro = bmi088gyro.readSensor();

        printf_(" High:{%f %f %f} ", accelHigh.x, accelHigh.y, accelHigh.z);
        printf_(" Pres:{%f %f} ", pressure.pressure, pressure.temperature);
        printf_(" Gyro:{%f %f %f} ", gyro.x, gyro.y, gyro.z);
        printf_(" Accel:{%f %f %f %lu} \n", accel.x, accel.y, accel.z, accel.time);
        


        /*rc = testADXLSpi();
        rc = testBMPSpi();
        rc = testBMIAccelSpi();
        rc = testBMIGyroSpi();*/

        (void)rc;

        /*DynamicJsonDocument doc(1024);

        JsonObject adxl375_json = doc.createNestedObject("ADXL375");
		adxl375_json["x"] = accelHigh.x;
		adxl375_json["y"] = accelHigh.y;
        adxl375_json["z"] = accelHigh.z;

	    JsonObject bmp388_json = doc.createNestedObject("BMP388");
		bmp388_json["pres"] = pressure.pressure;
        bmp388_json["temp"] = pressure.temperature;

        char string[1000];
		serializeJson(doc,string,sizeof(string));*/

        
    }
}