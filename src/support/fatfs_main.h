/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */
#ifndef FATFS_MAIN_H
#define FATFS_MAIN_H

#include "atmel_start.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <string.h>
#include <hal_calendar.h>
#include "ff.h"

extern struct calendar_descriptor CALENDER_INTERFACE;

DWORD get_fattime(void);

void fatfs_app_init(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // FATFS_MAIN_H
