/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */

#include "fatfs_main.h"

/**
 * \brief Current time returned is packed into a DWORD value.
 *
 * The bit field is as follows:
 *
 * bit31:25  Year from 1980 (0..127)
 *
 * bit24:21  Month (1..12)
 *
 * bit20:16  Day in month(1..31)
 *
 * bit15:11  Hour (0..23)
 *
 * bit10:5   Minute (0..59)
 *
 * bit4:0    Second (0..59)
 *
 * \return Current time.
 */
DWORD get_fattime(void)
{
	uint32_t                  ul_time;
	struct calendar_date_time datetime;
	calendar_get_date_time(&CALENDER_INTERFACE, &datetime);

	ul_time = ((datetime.date.year - 1980) << 25) | (datetime.date.month << 21) | (datetime.date.day << 16)
	          | (datetime.time.hour << 11) | (datetime.time.min << 5) | (datetime.time.sec << 0);
	return ul_time;
}

void fatfs_app_init(void)
{
	/*Low layer module initialisation to be added*/
}
