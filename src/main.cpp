#include <atmel_start.h>
#include "ADXL375.hpp"
#include "BMI088.hpp"
#include "ArduinoJson.h"
#include "sdtester.c"
#include "printf.h"
#include "BMP3xx.hpp"
#include "SPITest.hpp"

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	gpio_set_pin_level(LED1, true);
	gpio_set_pin_level(LED2, true);
	gpio_set_pin_level(LED3, true);
	gpio_set_pin_level(LED4, true);

	delay_ms(1000);

	gpio_set_pin_level(LED1, false);
	gpio_set_pin_level(LED2, false);
	gpio_set_pin_level(LED3, false);
	gpio_set_pin_level(LED4, false);

	spi_m_sync_set_mode(&SPI_SENSOR, SPI_MODE_3);
	spi_m_sync_enable(&SPI_SENSOR);

	ADXL375 adxl375(&SPI_SENSOR,ADXL_CS_1);
	adxl375.init();
	adxl375.setDataRate(ADXL375::BW_12_5HZ);
	adxl375.startMeasuring();

	BMI088Accel bmi088accel(&SPI_SENSOR,ACCEL_CS_1);
	bmi088accel.begin();

	BMI088Gyro bmi088gyro(&SPI_SENSOR,GYRO_CS_1);
	bmi088gyro.begin();

	BMP3xx pres(&SPI_SENSOR,BMP_CS_1);
	pres.begin();

	delay_ms(1000);

	while (true)
	{
		/*for(int j=0;j<20;j++){
			gpio_toggle_pin_level(LED4);
			for(int k=0;k<100;k++){
				for(int i=0;i<10000;i++){}
				gpio_toggle_pin_level(BUZZER);
			}
			printf_("Blink!\n");
		}*/
		

		//printf_("SD Test!");	
		//sdtester();

		BMI088Accel::Data accel = bmi088accel.readSensor();
		BMI088Gyro::Data gyro = bmi088gyro.readSensor();
		ADXL375::ADXL375_Data lol = adxl375.getXYZ();

		printf_("Pressure: %f\n", pres.readPressure());

		/*
		DynamicJsonDocument doc(1024);

		JsonObject bmi088_json = doc.createNestedObject("bmi088");
		bmi088_json["temp"] = accel.temp;
		bmi088_json["time"] = accel.time;

		JsonObject bmi_accel_json = bmi088_json.createNestedObject("accel");
		bmi_accel_json["x"] = accel.x;
		bmi_accel_json["y"] = accel.y;
		bmi_accel_json["z"] = accel.z;

		JsonObject bmi_gyro_json = bmi088_json.createNestedObject("gryo");
		bmi_gyro_json["x"] = gyro.x;
		bmi_gyro_json["y"] = gyro.y;
		bmi_gyro_json["z"] = gyro.z;

		JsonObject adxl375_json = doc.createNestedObject("adxl375");
		adxl375_json["x"] = lol.x;
		adxl375_json["y"] = lol.y;
		adxl375_json["z"] = lol.z;

		char string[1000];
		serializeJson(doc,string,sizeof(string));

		printf_("%s\n",string);*/

	}
}