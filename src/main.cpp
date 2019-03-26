#include <atmel_start.h>
#include "ADXL375.hpp"
#include "BMI088.hpp"
#include "ArduinoJson.h"
#include "sdtester.c"
#include "printf.h"
#include "BMP3xx.hpp"
#include "SPITest.hpp"
#include "Squib.hpp"

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


	spi_m_sync_disable(&SPI_SENSOR);
	spi_m_sync_set_mode(&SPI_SENSOR, SPI_MODE_3);
	spi_m_sync_enable(&SPI_SENSOR);

	spi_m_sync_disable(&SPI_SQUIB);
	spi_m_sync_set_mode(&SPI_SQUIB, SPI_MODE_3);
	spi_m_sync_enable(&SPI_SQUIB);

	ADXL375 adxl375(&SPI_SENSOR,ADXL_CS_1);
	adxl375.init();
	adxl375.startMeasuring();

	BMI088Accel bmi088accel(&SPI_SENSOR,ACCEL_CS_1);
	bmi088accel.begin();

	BMI088Gyro bmi088gyro(&SPI_SENSOR,GYRO_CS_1);
	bmi088gyro.begin();

	BMP3xx bmp388(&SPI_SENSOR,BMP_CS_1);
	bmp388.begin();

	delay_ms(1000);

	Squib squib(&SPI_SQUIB, SQUIB_CS);

	uint8_t leds[] = {LED1,LED2,LED3,LED4};

	uint8_t led = 0;

	bool fire = false;

	//adc_sync_enable_channel(&ADC_0,1);

	while (true)
	{
		gpio_toggle_pin_level(leds[led]);
		led++;
		led = led % 4;
		
		/*
		for(int k=0;k<100;k++){
			for(int i=0;i<10000;i++){}
			gpio_toggle_pin_level(BUZZER);
		}
		*/
		if(fire){
			squib.fire(CMD_FIRE_1A2A1B2B);
		}else{
			squib.fire(CMD_FIRE_NO_SQUIBS);
		}
		fire = !fire;

		//printf_("SD Test!");	
		//sdtester();

		uint8_t data[] = {0,0};		

		uint32_t test;

		data[0] = 0;
		data[1] = 0;

		//adc_sync_set_inputs(&ADC_0, 0x0D, 0x18, 1);

		//adc_sync_read_channel(&ADC_0,1,data,2);

		while( ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_INPUTCTRL ); //wait for sync
  		ADC0->INPUTCTRL.bit.MUXPOS = 14; // Selection for the positive ADC input
		ADC0->INPUTCTRL.bit.MUXNEG = 24;

		while( ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_SAMPCTRL ); //wait for sync
  		ADC0->SAMPCTRL.bit.SAMPLEN = 63;

  		ADC0->CTRLA.bit.PRESCALER = 7;

		while( ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE ); //wait for sync
		ADC0->CTRLA.bit.ENABLE = 0x01;             // Enable ADC

		// Start conversion
		while( ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE ); //wait for sync
		
		ADC0->SWTRIG.bit.START = 1;

		// Clear the Data Ready flag
		ADC0->INTFLAG.reg = ADC_INTFLAG_RESRDY;

		// Start conversion again, since The first conversion after the reference is changed must not be used.
		ADC0->SWTRIG.bit.START = 1;

		while (ADC0->INTFLAG.bit.RESRDY == 0);   // Waiting for conversion to complete

		//test = ;	

		printf_("AIN14: %032lb, ", ADC0->RESULT.reg);	
		printf_("AIN14: %u, ", ADC0->RESULT.reg);	

		while( ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE ); //wait for sync
  		ADC0->CTRLA.bit.ENABLE = 0x00;             // Disable ADC
  		while( ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE ); //wait for sync

		uint8_t measB = data[0];

		printf_("CTRLA: %u ", ADC0->CTRLA.reg);
		printf_("CTRLB: %u ", ADC0->CTRLB.reg);
		printf_("REFCTRL: %u ",ADC0->REFCTRL.reg);
		printf_("INPUTCTRL: %u ",ADC0->INPUTCTRL.reg);
		printf_("SAMPCTRL: %u ",ADC0->SAMPCTRL.reg);
		printf_("AVGCTRL: %u ",ADC0->AVGCTRL.reg);
		
		//data[0] = 0;
		//data[1] = 0;

		//adc_sync_set_inputs(&ADC_0, 0x0E, 0x18, 1);

		//adc_sync_read_channel(&ADC_0,1,data,2);

		//test = ((Adc*)ADC_0.device.hw)->RESULT.reg;

		//printf_("AIN14: %lu, ", test);

		uint8_t measA = data[0];

		float ratioA = 0.02726;
		float ratioB = 0.04868;

		float cellA = measA * ratioA;
		float cellB = (measB * ratioB) - cellA;

		printf("Cell A: %fV, ", cellA);
		printf("Cell B: %fV\n", cellB);

		squib.getStatus();
		Squib::Status status =  squib.status;
		BMI088Accel::Data accel = bmi088accel.readSensor();
		BMI088Gyro::Data gyro = bmi088gyro.readSensor();
		ADXL375::Data accelHigh = adxl375.readSensor();
		BMP3xx::Data pressure = bmp388.readSensor();

		DynamicJsonDocument doc(1024);

		/* JsonObject bmi088_json = doc.createNestedObject("BMI088");
		bmi088_json["temp"] = accel.temp;
		bmi088_json["time"] = accel.time;

		JsonObject bmi_accel_json = bmi088_json.createNestedObject("Accel");
		bmi_accel_json["x"] = accel.x;
		bmi_accel_json["y"] = accel.y;
		bmi_accel_json["z"] = accel.z;

		JsonObject bmi_gyro_json = bmi088_json.createNestedObject("Gryo");
		bmi_gyro_json["x"] = gyro.x;
		bmi_gyro_json["y"] = gyro.y;
		bmi_gyro_json["z"] = gyro.z;

		JsonObject adxl375_json = doc.createNestedObject("ADXL375");
		adxl375_json["x"] = accelHigh.x;
		adxl375_json["y"] = accelHigh.y;
		adxl375_json["z"] = accelHigh.z; */

		JsonObject bmp388_json = doc.createNestedObject("BMP388");
		bmp388_json["pres"] = pressure.pressure;
		bmp388_json["temp"] = pressure.temperature;

		/*JsonObject squib_json = doc.createNestedObject("Squib");
		squib_json["fen1"] = status.Squib_StatFen1;
		squib_json["fen2"] = status.Squib_StatFen2;
		squib_json["1A_res"] = status.Squib_Stat1AResistance;
		squib_json["1B_res"] = status.Squib_Stat1BResistance;
		squib_json["2A_res"] = status.Squib_Stat2AResistance;
		squib_json["2B_res"] = status.Squib_Stat2BResistance;*/

		char string[1000];
		serializeJson(doc,string,sizeof(string));

		//printf_("%s\n",string);
		//delay_ms(50);
	}
}