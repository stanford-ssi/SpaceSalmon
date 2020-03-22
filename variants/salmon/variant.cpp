#include "variant.h"

//Tim's Variants for Space Salmon

const PinDescription g_APinDescription[]=
{
  //0 NULL PIN
  { PORTB, 0, PIO_NOT_A_PIN, PIN_ATTR_PWM_G, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },
  //1-4 LEDs
  { PORTA, 30, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, TCC2_CH0, NOT_ON_TIMER, EXTERNAL_INT_14 },
  { PORTA, 31, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, TCC2_CH1, NOT_ON_TIMER, EXTERNAL_INT_15 },
  { PORTB, 30, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, TCC4_CH0, NOT_ON_TIMER, EXTERNAL_INT_14 },
  { PORTB, 31, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, TCC4_CH1, NOT_ON_TIMER, EXTERNAL_INT_15 },
  //5 Buzzer
  { PORTA, 15, PIO_TIMER, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, NOT_AN_INTERRUPT },
  //6-7 Bat Sense
  { PORTB, 1, PIO_DIGITAL, PIN_ATTR_DIGITAL, ADC_Channel13, NOT_ON_PWM, NOT_ON_TIMER, NOT_AN_INTERRUPT },
  { PORTB, 2, PIO_DIGITAL, PIN_ATTR_DIGITAL, ADC_Channel14, NOT_ON_PWM, NOT_ON_TIMER, NOT_AN_INTERRUPT },
  //8-10 Sensor SPI
  { PORTA, 4, PIO_SERCOM_ALT, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, NOT_AN_INTERRUPT }, // MISO: SERCOM0/PAD[0]
  { PORTA, 5, PIO_SERCOM_ALT, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, NOT_AN_INTERRUPT }, // SCK: SERCOM0/PAD[1]
  { PORTA, 7, PIO_SERCOM_ALT, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, NOT_AN_INTERRUPT }, // MOSI: SERCOM0/PAD[3]
  //11-14 Sensor CS
  { PORTB, 6, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, NOT_AN_INTERRUPT }, // ADXL_CS_1
  { PORTB, 7, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, NOT_AN_INTERRUPT }, // BMP_CS_1
  { PORTB, 8, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, NOT_AN_INTERRUPT }, // GYRO_CS_1
  { PORTB, 9, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, NOT_AN_INTERRUPT }, // ACCEL_CS_1
  //15-18
  { PORTA, 2, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, NOT_AN_INTERRUPT }, // ADXL_CS_2
  { PORTA, 3, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, NOT_AN_INTERRUPT }, // BMP_CS_2
  { PORTB, 4, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, NOT_AN_INTERRUPT }, // GYRO_CS_2
  { PORTB, 5, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, NOT_AN_INTERRUPT }, // ACCEL_CS_2
} ;

const void* g_apTCInstances[TCC_INST_NUM+TC_INST_NUM]={ TCC0, TCC1, TCC2, TCC3, TCC4, TC0, TC1, TC2, TC3, TC4, TC5 } ;
const uint32_t GCLK_CLKCTRL_IDs[TCC_INST_NUM+TC_INST_NUM] = { TCC0_GCLK_ID, TCC1_GCLK_ID, TCC2_GCLK_ID, TCC3_GCLK_ID, TCC4_GCLK_ID, TC0_GCLK_ID, TC1_GCLK_ID, TC2_GCLK_ID, TC3_GCLK_ID, TC4_GCLK_ID, TC5_GCLK_ID } ;

SERCOM sercom0( SERCOM0 ) ;
SERCOM sercom1( SERCOM1 ) ;
SERCOM sercom2( SERCOM2 ) ;
SERCOM sercom3( SERCOM3 ) ;
SERCOM sercom4( SERCOM4 ) ;
SERCOM sercom5( SERCOM5 ) ;
