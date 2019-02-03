/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef ATMEL_START_PINS_H_INCLUDED
#define ATMEL_START_PINS_H_INCLUDED

#include <hal_gpio.h>

// SAME51 has 14 pin functions

#define GPIO_PIN_FUNCTION_A 0
#define GPIO_PIN_FUNCTION_B 1
#define GPIO_PIN_FUNCTION_C 2
#define GPIO_PIN_FUNCTION_D 3
#define GPIO_PIN_FUNCTION_E 4
#define GPIO_PIN_FUNCTION_F 5
#define GPIO_PIN_FUNCTION_G 6
#define GPIO_PIN_FUNCTION_H 7
#define GPIO_PIN_FUNCTION_I 8
#define GPIO_PIN_FUNCTION_J 9
#define GPIO_PIN_FUNCTION_K 10
#define GPIO_PIN_FUNCTION_L 11
#define GPIO_PIN_FUNCTION_M 12
#define GPIO_PIN_FUNCTION_N 13


//USB
#define USB_DM GPIO(GPIO_PORTA, 24)
#define USB_DP GPIO(GPIO_PORTA, 25)

//SD Card
#define SD_CD GPIO(GPIO_PORTA, 6)
#define SD_CMD GPIO(GPIO_PORTA, 8)
#define SD_DAT0 GPIO(GPIO_PORTA, 9)
#define SD_DAT1 GPIO(GPIO_PORTA, 10)
#define SD_DAT2 GPIO(GPIO_PORTA, 11)
#define SD_DAT3 GPIO(GPIO_PORTB, 10)
#define SD_SCK GPIO(GPIO_PORTB, 11)

//Sensor SPI
#define SENSOR_MISO GPIO(GPIO_PORTA, 4)
#define SENSOR_SCK GPIO(GPIO_PORTA, 5)
#define SENSOR_MOSI GPIO(GPIO_PORTA, 7)

//Sensor 1
#define ADXL_CS_1 GPIO(GPIO_PORTB, 6)
#define BMP_CS_1 GPIO(GPIO_PORTB, 7)
#define GYRO_CS_1 GPIO(GPIO_PORTB, 8)
#define ACCEL_CS_1 GPIO(GPIO_PORTB, 9)

#define ADXL_INT_1 GPIO(GPIO_PORTB, 22)
#define ACCEL_INT_1 GPIO(GPIO_PORTB, 23)
#define GYRO_INT_1 GPIO(GPIO_PORTA, 27)

//Sensor 2
#define ADXL_CS_2 GPIO(GPIO_PORTA, 2)
#define BMP_CS_2 GPIO(GPIO_PORTA, 3)
#define GYRO_CS_2 GPIO(GPIO_PORTB, 4)
#define ACCEL_CS_2 GPIO(GPIO_PORTB, 5)

#define ADXL_INT_2 GPIO(GPIO_PORTA, 14)
#define ACCEL_INT_2 GPIO(GPIO_PORTB, 16)
#define GYRO_INT_2 GPIO(GPIO_PORTB, 17)

//Blink Beep Doot Doot
#define LED1 GPIO(GPIO_PORTA, 30)
#define LED2 GPIO(GPIO_PORTA, 31)
#define LED3 GPIO(GPIO_PORTB, 30)
#define LED4 GPIO(GPIO_PORTB, 31)
#define BUZZER GPIO(GPIO_PORTB,0)

//Battery Sense
#define BAT_SNS_B GPIO(GPIO_PORTB,1)
#define BAT_SNS_A GPIO(GPIO_PORTB,2)

#endif // ATMEL_START_PINS_H_INCLUDED
