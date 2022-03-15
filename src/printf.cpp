#include "Arduino.h"

extern "C" void tim_print(char*, int);

void tim_print(char* buf, int len){
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

        tim_print((char *)ptr, len);

        return n;
    }
}