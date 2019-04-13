/**
 * \file
 *
 * \brief FATFS POSIX example application
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#include "FATFSProj_main.h"

/** Size of the file to write/read. */
#define DATA_SIZE 2048

/** Test settings: Number of bytes to test */
#define TEST_SIZE (4 * 1024)

/** Uncomment the macro to Format card and do testing*/
#define FORMAT_CARD

/* Read/write buffer */
static uint8_t data_buffer[DATA_SIZE];

/**
 * \brief Scan files under a certain path.
 *
 * \param path Folder path.
 *
 * \return Scan result, 1: success.
 */
static FRESULT scan_files(char *path)
{
	FRESULT res;
	FILINFO fno;
	DIR     dir;
	int32_t i;
	char *  pc_fn;
#if _USE_LFN
	char c_lfn[_MAX_LFN + 1];
	fno.lfname = c_lfn;
	fno.lfsize = sizeof(c_lfn);
#endif

	/* Open the directory */
	res = f_opendir(&dir, path);
	if (res == FR_OK) {
		i = strlen(path);
		for (;;) {
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0) {
				break;
			}

#if _USE_LFN
			pc_fn = *fno.lfname ? fno.lfname : fno.fname;
#else
			pc_fn = fno.fname;
#endif
			if (*pc_fn == '.') {
				continue;
			}

			/* Check if it is a directory type */
			if (fno.fattrib & AM_DIR) {
				sprintf(&path[i], "/%s", pc_fn);
				res = scan_files(path);
				if (res != FR_OK) {
					break;
				}

				path[i] = 0;
			} else {
				printf("%s/%s\n\r", path, pc_fn);
			}
		}
	}

	return res;
}

/**
 * \brief Do file system tests.
 *
 * \param disk_dev_num disk number
 *
 * \return Test result, 1: success.
 */
static uint8_t run_fatfs_test(uint32_t disk_dev_num)
{
	uint32_t i;
	UINT     byte_to_read;
	UINT     byte_read;
#if _FS_READONLY == 0
	UINT byte_written;
#endif

	FRESULT res;
	DIR     dirs;
	TCHAR   root_directory[4];
	/* File name to be validated */
	TCHAR file_name[18];

	TCHAR test_folder_name[5];
	TCHAR test_folder_location[8];
	TCHAR get_test_folder[8];
#if _USE_LFN
	char lfn[_MAX_LFN + 1];
	fno.lfname = lfn;
	fno.lfsize = _MAX_LFN + 1;
#endif

	/* Declare these as static to avoid stack usage.
	 * They each contain an array of maximum sector size.
	 */
	static FATFS fs;
	static FIL   file_object;
	char         disk_str_num[2];

	sprintf(disk_str_num, "%d", disk_dev_num);
	sprintf(test_folder_name, "%s", (const char *)"TEST");
	sprintf(root_directory, "%s:/", disk_str_num);
	sprintf(test_folder_location, "%s%s", root_directory, test_folder_name);
	sprintf(file_name, "%s/Basic.bin", test_folder_location); /*File path*/

	/* Mount disk*/
	printf("-I- Mount disk %d\n\r", (int)disk_dev_num);
	/* Clear file system object */
	memset(&fs, 0, sizeof(FATFS));
	res = f_mount(&fs, "", 1);
	if (res != FR_OK) {
		printf("-E- f_mount pb: 0x%X\n\r", res);
		return 0;
	}

#ifdef FORMAT_CARD /** Enable the macro to Format card and do testing*/
#if _FS_READONLY == 0
	/* Format disk */
	printf("-I- Format disk %d\n\r", (int)disk_dev_num);
	printf("-I- Please wait a moment during formatting...\r");
	res = f_mkfs(disk_str_num, /* Drv */
	             0,            /* FDISK partition */
	             512);         /* AllocSize */
	printf("-I- Disk format finished !\r");
	if (res != FR_OK) {
		printf("-E- f_mkfs pb: 0x%X\n\r", res);
		return 0;
	}
#else
	printf("-I- Please run Full version FAT FS test first\r");
	return 0;
#endif
#endif

	/*Create a directory*/
	printf("-I- Creating test directory !\r");
	res = f_mkdir(test_folder_name);
	if (res != FR_OK) {
		printf("-E- f_mkdir pb: 0x%X\n\r", res);
		return 0;
	}

	/*Opening the directory*/
	printf("-I- Opening directory !\r");
	res = f_opendir(&dirs, test_folder_location);
	if (res == FR_OK) {
		/* Erase sd card to reformat it ? */
		printf("-I- The disk is already formatted.\r");

		/* Display the file tree */
		printf("-I- Display files contained in the memory :\r");
		strcpy((char *)data_buffer, test_folder_location);
		scan_files((char *)data_buffer);
	}

	/*Changing  the directory*/
	printf("-I- Changing to test directory !\r");
	res = f_chdir(test_folder_location);
	printf("-I- Changed to directory\r");
	if (res != FR_OK) {
		printf("-E- f_chdir pb: 0x%X\n\r", res);
		return 0;
	}

	/*Get the current working directory*/
	printf("-I- Getting current working directory !\r");
	res = f_getcwd(get_test_folder, 8);
	if (res != FR_OK) {
		printf("-E- getcwd not working\n\r");
		return 0;
	}
	res = strcmp(test_folder_location, get_test_folder);
	if (res != FR_OK) {
		printf("-E- Mismatch in f_getcwd and actual Folder name\n\r");
		return 0;
	}

#if _FS_READONLY == 0
	/* Create a new file */
	printf("-I- Create a file : \"%s\"\n\r", file_name);
	res = f_open(&file_object, (char const *)file_name, FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
	if (res != FR_OK) {
		printf("-E- f_open create pb: 0x%X\n\r", res);
		return 0;
	}

	/* Write a checkerboard pattern in the buffer */
	for (i = 0; i < sizeof(data_buffer); i++) {
		if ((i & 1) == 0) {
			data_buffer[i] = (i & 0x55);
		} else {
			data_buffer[i] = (i & 0xAA);
		}
	}
	printf("-I- Write file\r");
	for (i = 0; i < TEST_SIZE; i += DATA_SIZE) {
		res = f_write(&file_object, data_buffer, DATA_SIZE, &byte_written);

		if (res != FR_OK) {
			printf("-E- f_write pb: 0x%X\n\r", res);
			return 0;
		}
	}

	/* Flush after writing */
	printf("-I- Synching file\r");
	res = f_sync(&file_object);
	if (res != FR_OK) {
		printf("-E- f_sync pb: 0x%X\n\r", res);
		return 0;
	}
	/* Close the file */
	printf("-I- Close file\r");
	res = f_close(&file_object);
	if (res != FR_OK) {
		printf("-E- f_close pb: 0x%X\n\r", res);
		return 0;
	}
#endif
	/* Open the file */
	printf("-I- Open the same file : \"%s\"\n\r", file_name);
	res = f_open(&file_object, (char const *)file_name, FA_OPEN_EXISTING | FA_READ);
	if (res != FR_OK) {
		printf("-E- f_open read pb: 0x%X\n\r", res);
		return 0;
	}

	/* Read file */
	printf("-I- Read file\r");
	memset(data_buffer, 0, DATA_SIZE);
	byte_to_read = file_object.fsize;

	for (i = 0; i < byte_to_read; i += DATA_SIZE) {
		res = f_read(&file_object, data_buffer, DATA_SIZE, &byte_read);
		if (res != FR_OK) {
			printf("-E- f_read pb: 0x%X\n\r", res);
			return 0;
		}
	}

	/* Close the file*/
	printf("-I- Close file\r");
	res = f_close(&file_object);
	if (res != FR_OK) {
		printf("-E- f_close pb: 0x%X\n\r", res);
		return 0;
	}

	/* Compare the read data with the expected data */
	for (i = 0; i < sizeof(data_buffer); i++) {
		if (!((((i & 1) == 0) && (data_buffer[i] == (i & 0x55))) || (data_buffer[i] == (i & 0xAA)))) {
			printf("Invalid data at data[%u] (expected 0x%02X, read 0x%02X)\n\r",
			       (unsigned int)i,
			       (unsigned int)(((i & 1) == 0) ? (i & 0x55) : (i & 0xAA)),
			       (unsigned int)data_buffer[i]);
		}
	}
	printf("-I- File data Ok !\r");

#if _FS_READONLY == 0
	/*Rename the file*/
	printf("-I- Rename file\r");
	res = f_rename(file_name, "Renam.bin");
	if (res != FR_OK) {
		printf("-E- f_rename pb: 0x%X\n\r", res);
		return 0;
	}

	memset(data_buffer, 0, DATA_SIZE);
	/* Display the file tree */
	printf("-I- Display files contained in the memory after renaming :\r");
	strcpy((char *)data_buffer, test_folder_location);
	scan_files((char *)data_buffer);

	memset(file_name, 0, sizeof(file_name));
	sprintf(file_name, "%s/Renam.bin", test_folder_location);

	/*Change the mode of the file to hidden*/
	printf("-I- Change file mode\r");
	res = f_chmod(file_name, AM_HID, 0);
	if (res != FR_OK) {
		printf("-E- f_chmod pb: 0x%X\n\r", res);
		return 0;
	}

	memset(data_buffer, 0, DATA_SIZE);
	/* Display the file tree */
	printf("-I- Display files contained in the memory after hiding :\r");
	strcpy((char *)data_buffer, test_folder_location);
	scan_files((char *)data_buffer);

#if _USE_FIND == 1
	/*Searching a file in location*/
	res = f_findfirst(&dirs, &fno, "", "Ren*.bin"); /* Start to search for bin files with the name started by "Ren" */
	if (res != FR_OK) {
		printf("-E- f_findfirst pb: 0x%X\n\r", res);
		return 0;
	}
#if _USE_LFN
	printf("-I  file name search found %-12s  %s\n", fno.fname, fno.lfname); /* Display the item name */
#else
	printf("-I  file name search found %s\n", fno.fname); /* Display the item name */
#endif
#endif /*_USE_FIND*/

#if _USE_LABEL == 1
	/*Setting the drive label to ACME*/
	printf("-I- Setting label Name\r");
	res = f_setlabel((const char *)"ACME");
	if (res != FR_OK) {
		printf("-E- f_setlabel pb: 0x%X\n\r", res);
		return 0;
	}
	/*Getting the drive label*/
	printf("-I- Getting label Name\r");
	res = f_getlabel(file_name, &label_name, 0);
	if (res != FR_OK) {
		printf("-E- f_getlabel pb: 0x%X\n\r", res);
		return 0;
	}
	if (strcmp((const char *)"ACME", (const char *)label_name)) {
		printf("-E- f_getlabel/f_setlabel not working pb: 0x%X\n\r", res);
	}
#endif
	printf("-I- Delete file\r");
	res = f_unlink(file_name);
	if (res != FR_OK) {
		printf("-E- f_unlink pb: 0x%X\n\r", res);
		return 0;
	}
	memset(data_buffer, 0, DATA_SIZE);
	/* Display the file tree */
	printf("-I- Display files contained in the memory after deleting :\r");
	strcpy((char *)data_buffer, test_folder_location);
	scan_files((char *)data_buffer);
#endif
	/*Closing directory*/
	res = f_closedir(&dirs);
	if (res != FR_OK) {
		printf("-E- f_closedir pb: 0x%X\n\r", res);
		return 0;
	}
	return 1;
}

int main(void)
{
	atmel_start_init();

	for(uint32_t i=0; i<1000000000; i++){
		printf_("1");
	}
	
	printf_("1");

	for(uint32_t i=0; i<100000; i++){}

	printf("test");

	if (run_fatfs_test(0)) {
		printf("-I- DISK 0 Test passed !\n\r\n\r");
	} else {
		printf("-F- DISK 0 Test Failed !\n\r\n\r");
	}
	while (1)
		;
	return 0;
}
