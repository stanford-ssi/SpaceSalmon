#include <atmel_start.h>
#include <usb_start.h>

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	volatile int lol;

	cdcd_acm_example();
	/* Replace with your application code */
	
	
	
	while(true){
		for(int j=0;j<20;j++){
			
			gpio_toggle_pin_level(LED4);
			
			for(int k=0;k<100;k++){
				for(int i=0;i<10000;i++){
					lol = i;
				}
				gpio_toggle_pin_level(BUZZER);
			}
			printf("TopKekTest\n");
		}
		sdtester();
	}
		
}
