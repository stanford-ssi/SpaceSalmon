#include "PyroFets.h"
#include "hal_gpio.h"

PyroFets::PyroFets(uint8_t fire1, uint8_t sense1, uint8_t fire2, uint8_t sense2){
    fire1_pin = fire1;
    sense1_pin = sense1;
    fire2_pin = fire2;
    sense2_pin = sense2;

    gpio_set_pin_level(fire1_pin, false);
	gpio_set_pin_direction(fire1_pin, GPIO_DIRECTION_OFF);
	gpio_set_pin_function(fire1_pin, GPIO_PIN_FUNCTION_OFF);

    gpio_set_pin_level(fire2_pin, false);
	gpio_set_pin_direction(fire2_pin, GPIO_DIRECTION_OFF);
	gpio_set_pin_function(fire2_pin, GPIO_PIN_FUNCTION_OFF);

    gpio_set_pin_level(sense1_pin, false);
	gpio_set_pin_direction(sense1_pin, GPIO_DIRECTION_OFF);
	gpio_set_pin_function(sense1_pin, GPIO_PIN_FUNCTION_OFF);

    gpio_set_pin_level(sense2_pin, false);
	gpio_set_pin_direction(sense2_pin, GPIO_DIRECTION_OFF);
	gpio_set_pin_function(sense2_pin, GPIO_PIN_FUNCTION_OFF);

}

void PyroFets::arm(){
    armed = true;
    gpio_set_pin_level(fire1_pin, false);
    gpio_set_pin_level(fire2_pin, false);
    gpio_set_pin_direction(fire1_pin, GPIO_DIRECTION_OUT);
    gpio_set_pin_direction(fire2_pin, GPIO_DIRECTION_OUT);
}

void PyroFets::disarm(){
    armed = false;
    gpio_set_pin_level(fire1_pin, false);
    gpio_set_pin_level(fire2_pin, false);
    gpio_set_pin_direction(fire1_pin, GPIO_DIRECTION_OFF);
    gpio_set_pin_direction(fire2_pin, GPIO_DIRECTION_OFF);
}

bool PyroFets::fire(PyroChannel channel){
    if(armed){
        if(channel == SquibA){
            gpio_set_pin_level(fire2_pin, false);
            gpio_set_pin_level(fire1_pin, true);
        }else if(channel == SquibB){
            gpio_set_pin_level(fire1_pin, false);
            gpio_set_pin_level(fire2_pin, true);
        }else if (channel == SquibNone){
            gpio_set_pin_level(fire1_pin, false);
            gpio_set_pin_level(fire2_pin, false);
        }
        return true;
    }else{
        return false;
    }
}

void PyroFets::getStatus(){
    
}