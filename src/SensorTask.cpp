#include "SensorTask.hpp"

TaskHandle_t SensorTask::taskHandle = NULL;
StaticTask_t SensorTask::xTaskBuffer;
StackType_t SensorTask::xStack[stackSize];

SensorTask::SensorTask()
{
    SensorTask::taskHandle = xTaskCreateStatic(activity,                  //static function to run
                                               "Sensors",                 //task name
                                               stackSize,                 //stack depth (words!)
                                               NULL,                      //parameters
                                               1,                         //priority
                                               SensorTask::xStack,        //stack object
                                               &SensorTask::xTaskBuffer); //TCB object
}

TaskHandle_t SensorTask::getTaskHandle()
{
    return taskHandle;
}

void SensorTask::activity(void *ptr)
{
    sys.tasks.logger.log("Initializing Sensors");

    int rc;
    char str[100];

    vTaskDelay(2); //but why...

    sys.sensors.adxl1.init();
    sys.sensors.adxl1.startMeasuring();

    vTaskDelay(2); //but why...

    sys.sensors.adxl2.init();
    sys.sensors.adxl2.startMeasuring();

    vTaskDelay(2); //but why...

    rc = sys.sensors.pres1.init();

    if (rc != true)
    {
        sys.tasks.logger.log("Error Starting BMP1");
    }

    rc = sys.sensors.pres2.init();

    if (rc != true)
    {
        sys.tasks.logger.log("Error Starting BMP2");
    }

    gpio_set_pin_level(ACCEL_CS_1, false);
    vTaskDelay(1);
    gpio_set_pin_level(ACCEL_CS_1, true);
    vTaskDelay(1);

    rc = sys.sensors.imu1.accel->begin();

    if (rc != 1)
    {
        snprintf(str, sizeof(str), "Error Starting BMI1Accel: %i", rc);
        sys.tasks.logger.log(str);
    }

    gpio_set_pin_level(ACCEL_CS_2, false);
    vTaskDelay(1);
    gpio_set_pin_level(ACCEL_CS_2, true);
    vTaskDelay(1);

    rc = sys.sensors.imu2.accel->begin();

    if (rc != 1)
    {
        snprintf(str, sizeof(str), "Error Starting BMI2Accel: %i", rc);
        sys.tasks.logger.log(str);
    }

    vTaskDelay(2); //but why...

    rc = sys.sensors.imu1.gyro->begin();

    
    if (rc != 1)
    {
        snprintf(str, sizeof(str), "Error Starting BMI1Gyro: %i", rc);
        sys.tasks.logger.log(str);
    }

    vTaskDelay(2); //but why...

    rc = sys.sensors.imu2.gyro->begin();

    if (rc != 1)
    {
        snprintf(str, sizeof(str), "Error Starting BMI2Gyro: %i", rc);
        sys.tasks.logger.log(str);
    }

    TickType_t lastSensorTime = xTaskGetTickCount();

    while (true)
    {
        vTaskDelayUntil(&lastSensorTime, 10);

        gpio_set_pin_level(SENSOR_LED, true);

        StaticJsonDocument<1024> sensor_json;

        SensorData data;

        data.adxl1_data = sys.sensors.adxl1.readSensor();
        data.bmp1_data = sys.sensors.pres1.readSensor();
        //vTaskDelay(2); //but why...
        data.bmiaccel1_data = sys.sensors.imu1.accel->readSensor();
        //vTaskDelay(2); //but why...
        data.bmigyro1_data = sys.sensors.imu1.gyro->readSensor();

        //vTaskDelay(4); //but why...

        data.adxl2_data = sys.sensors.adxl2.readSensor();
        data.bmp2_data = sys.sensors.pres2.readSensor();
        //vTaskDelay(2); //but why...
        data.bmiaccel2_data = sys.sensors.imu2.accel->readSensor();
        //vTaskDelay(2); //but why...
        data.bmigyro2_data = sys.sensors.imu2.gyro->readSensor();

        data.tick = xTaskGetTickCount();

        sensor_json["tick"] = data.tick;

        JsonObject bmp1_json = sensor_json.createNestedObject("bmp1");
        JsonObject bmi1_json = sensor_json.createNestedObject("bmi1");
        JsonObject adxl1_json = sensor_json.createNestedObject("adxl1");

        bmp1_json["p"] = data.bmp1_data.pressure;
        bmp1_json["t"] = data.bmp1_data.temperature;

        JsonArray bmi1_accel_json = bmi1_json.createNestedArray("a");
        bmi1_accel_json.add(data.bmiaccel1_data.x);
        bmi1_accel_json.add(data.bmiaccel1_data.y);
        bmi1_accel_json.add(data.bmiaccel1_data.z);

        JsonArray bmi1_gyro_json = bmi1_json.createNestedArray("g");
        bmi1_gyro_json.add(data.bmigyro1_data.x);
        bmi1_gyro_json.add(data.bmigyro1_data.y);
        bmi1_gyro_json.add(data.bmigyro1_data.z);

        JsonArray adxl1_accel_json = adxl1_json.createNestedArray("a");
        adxl1_accel_json.add(data.adxl1_data.x);
        adxl1_accel_json.add(data.adxl1_data.y);
        adxl1_accel_json.add(data.adxl1_data.z);


        //second set!

        JsonObject bmp2_json = sensor_json.createNestedObject("bmp2");
        JsonObject bmi2_json = sensor_json.createNestedObject("bmi2");
        JsonObject adxl2_json = sensor_json.createNestedObject("adxl2");

        bmp2_json["p"] = data.bmp2_data.pressure;
        bmp2_json["t"] = data.bmp2_data.temperature;

        JsonArray bmi2_accel_json = bmi2_json.createNestedArray("a");
        bmi2_accel_json.add(data.bmiaccel2_data.x);
        bmi2_accel_json.add(data.bmiaccel2_data.y);
        bmi2_accel_json.add(data.bmiaccel2_data.z);

        JsonArray bmi2_gyro_json = bmi2_json.createNestedArray("g");
        bmi2_gyro_json.add(data.bmigyro2_data.x);
        bmi2_gyro_json.add(data.bmigyro2_data.y);
        bmi2_gyro_json.add(data.bmigyro2_data.z);

        JsonArray adxl2_accel_json = adxl2_json.createNestedArray("a");
        adxl2_accel_json.add(data.adxl2_data.x);
        adxl2_accel_json.add(data.adxl2_data.y);
        adxl2_accel_json.add(data.adxl2_data.z);
        

        sys.tasks.logger.logJSON(sensor_json, "sensor");

        gpio_set_pin_level(SENSOR_LED, false);
    }
}