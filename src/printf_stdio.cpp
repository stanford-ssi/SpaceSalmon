#include "Arduino.h"
#include "printf_stdio.hpp"

extern "C" void write_stdio(char*, int);

void setup_printf_stdio(void){
    Serial.begin(9600);
}

void write_stdio(char* buf, int len){
	Serial.write(buf,len);
}

extern "C" {

    #include <stdio.h>

    int __attribute__((weak)) _write(int file, char *ptr, int len); /* Remove GCC compiler warning */

    int __attribute__((weak)) _write(int file, char *ptr, int len)
    {
        int n = 0;

        if ((file != 1) && (file != 2) && (file != 3)) {
            return -1;
        }

        write_stdio((char *)ptr, len);

        return n;
    }
}