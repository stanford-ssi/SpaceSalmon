#include <stdio.h>

void tim_print2 (char*, int);

int __attribute__((weak)) _write(int file, char *ptr, int len); /* Remove GCC compiler warning */

int __attribute__((weak)) _write(int file, char *ptr, int len)
{
	int n = 0;

	if ((file != 1) && (file != 2) && (file != 3)) {
		return -1;
	}

    tim_print2((const uint8_t *)ptr, len);

	return n;
}