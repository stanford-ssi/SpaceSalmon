#include <atmel_start.h>
#include <usb_start.h>

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	cdcd_acm_example();
	/* Replace with your application code */
	
	gpio_set_pin_level(LED1,true);
	gpio_set_pin_level(LED2,true);
	gpio_set_pin_level(LED3,true);
	gpio_set_pin_level(LED4,true);
	
	for(int i=0;i<10000000;i++){}

	gpio_set_pin_level(LED1,false);
	gpio_set_pin_level(LED2,false);
	gpio_set_pin_level(LED3,false);
	gpio_set_pin_level(LED4,false);
	
	while(true){
		for(int j=0;j<20;j++){
			
			gpio_toggle_pin_level(LED4);
			
			for(int k=0;k<100;k++){
				for(int i=0;i<10000;i++){}
				gpio_toggle_pin_level(BUZZER);
			}
			printf("TopKekTest\n");
		}
		sdtester();
	}
		
}
