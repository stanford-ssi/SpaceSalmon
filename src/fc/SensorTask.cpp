#include "SensorTask.hpp"
#include "main.hpp"

SensorTask::SensorTask(uint8_t priority) : Task(priority, "Sensor") {}

void SensorTask::activity()
{
    if (sys.shitl)
    {
        vTaskSuspend(getTaskHandle());
    }

    sys.tasks.logger.log("Initializing Sensors");

    pinMode(11, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);
    pinMode(14, OUTPUT);
    pinMode(15, OUTPUT);
    pinMode(16, OUTPUT);
    pinMode(17, OUTPUT);
    pinMode(18, OUTPUT);

    digitalWrite(11, HIGH);
    digitalWrite(12, HIGH);
    digitalWrite(13, HIGH);
    digitalWrite(14, HIGH);
    digitalWrite(15, HIGH);
    digitalWrite(16, HIGH);
    digitalWrite(17, HIGH);
    digitalWrite(18, HIGH);

    sys.sensors.spi.begin();
    sys.sensors.spi.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE3));

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

    if (rc != BMP3_OK)
    {
        snprintf(str, sizeof(str), "Error Starting BMP1: %i", rc);
        sys.tasks.logger.log(str);
    }

    rc = sys.sensors.pres2.init();

    if (rc != BMP3_OK)
    {
        snprintf(str, sizeof(str), "Error Starting BMP2: %i", rc);
        sys.tasks.logger.log(str);
    }

    digitalWrite(14, false);
    vTaskDelay(1);
    digitalWrite(14, true);
    vTaskDelay(1);

    rc = sys.sensors.imu1.accel->begin();

    if (rc != 1)
    {
        snprintf(str, sizeof(str), "Error Starting BMI1Accel: %i", rc);
        sys.tasks.logger.log(str);
    }

    digitalWrite(18, false);
    vTaskDelay(1);
    digitalWrite(18, true);
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

        digitalWrite(SENSOR_LED, true);

        StaticJsonDocument<1024> sensor_json;

        SensorData data;

        data.tick = xTaskGetTickCount();

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

        sys.tasks.filter.queueSensorData(data);

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

        digitalWrite(SENSOR_LED, false);
    }
}