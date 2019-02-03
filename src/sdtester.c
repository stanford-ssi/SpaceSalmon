#include "atmel_start.h"
#include "sd_mmc.h"
#include "sd_mmc_protocol.h"
#include <string.h>

/* Offset before the end of memory card to start the test */
#define TEST_MEM_START_OFFSET (1024lu * 1024lu * 4lu)

/* Memory area size dedicated for the read/write test */
#define TEST_MEM_AREA_SIZE (1024lu * 1024lu * 1lu)

/**
 * Size of each read or write access.
 * Increasing this number can get higher R/W performance.
 */
#define TEST_MEM_ACCESS_SIZE (8lu * SD_MMC_BLOCK_SIZE)

#if TEST_MEM_START_OFFSET < TEST_MEM_AREA_SIZE
#error TEST_MEM_START_OFFSET must be higher than TEST_MEM_AREA_SIZE
#endif
#if TEST_MEM_ACCESS_SIZE > TEST_MEM_AREA_SIZE
#error TEST_MEM_AREA_SIZE must be higher than TEST_MEM_ACCESS_SIZE
#endif

/* The value used to generate test data */
#define TEST_FILL_VALUE_U32 (0x5500AAFFU)

/* Read and write test length of CIA in bytes */
#define TEST_CIA_SIZE (0x16)

/* Buffer used by read/write tests */
COMPILER_ALIGNED(4)
static uint8_t buf_test[TEST_MEM_ACCESS_SIZE];

COMPILER_ALIGNED(4)
/* Buffer for test SDIO data */
static uint8_t buf_cia[TEST_CIA_SIZE];

/**
 * \brief Dump and print buffer.
 *
 * \param data_buffer Pointer to data buffer.
 * \param length      Buffer length.
 */
static void main_dump_buffer(uint8_t *data_buffer, uint32_t length)
{
	uint32_t i;

	printf("Dump buffer (length=%d):", (int)length);
	for (i = 0; i < length; i++) {
		if ((i % 16) == 0) { /* Display 16 data per line */
			printf("\r\n%3x:", (int)i);
		}

		printf(" %02x", data_buffer[i]);
	}
	printf("\r\n");
}

/**
 * \brief Card R/W tests
 *
 * \param slot   SD/MMC slot to test
 */
static void main_test_memory(uint8_t slot)
{
	uint32_t last_blocks_addr, i, nb_trans;

	/* Compute the last address */
	last_blocks_addr = sd_mmc_get_capacity(slot) * (1024 / SD_MMC_BLOCK_SIZE);
	if (last_blocks_addr < (TEST_MEM_START_OFFSET / 512lu)) {
		printf("[Memory is too small.]\r\n");
		return;
	}
	last_blocks_addr -= (TEST_MEM_START_OFFSET / SD_MMC_BLOCK_SIZE);

	printf("Card R/W test:\r\n");

	/* Read the last block */
	printf("    Read... ");
	if (SD_MMC_OK != sd_mmc_init_read_blocks(slot, last_blocks_addr, TEST_MEM_AREA_SIZE / SD_MMC_BLOCK_SIZE)) {
		printf("[FAIL]\r\n");
		return;
	}
	for (nb_trans = 0; nb_trans < (TEST_MEM_AREA_SIZE / TEST_MEM_ACCESS_SIZE); nb_trans++) {
		if (SD_MMC_OK != sd_mmc_start_read_blocks(buf_test, TEST_MEM_ACCESS_SIZE / SD_MMC_BLOCK_SIZE)) {
			printf("[FAIL]\r\n");
			return;
		}
		if (SD_MMC_OK != sd_mmc_wait_end_of_read_blocks(false)) {
			printf("[FAIL]\r\n");
			return;
		}
	}
	printf("[OK]\r\n");

	/* Fill buffer with a pattern */
	for (i = 0; i < (TEST_MEM_ACCESS_SIZE / sizeof(uint32_t)); i++) {
		((uint32_t *)buf_test)[i] = TEST_FILL_VALUE_U32;
	}

	printf("    Write pattern... ");
	if (SD_MMC_OK != sd_mmc_init_write_blocks(slot, last_blocks_addr, TEST_MEM_AREA_SIZE / SD_MMC_BLOCK_SIZE)) {
		printf("[FAIL]\r\n");
		return;
	}

	for (nb_trans = 0; nb_trans < (TEST_MEM_AREA_SIZE / TEST_MEM_ACCESS_SIZE); nb_trans++) {
		((uint32_t *)buf_test)[0] = nb_trans;
		if (SD_MMC_OK != sd_mmc_start_write_blocks(buf_test, TEST_MEM_ACCESS_SIZE / SD_MMC_BLOCK_SIZE)) {
			printf("[FAIL]\r\n");
			return;
		}
		if (SD_MMC_OK != sd_mmc_wait_end_of_write_blocks(false)) {
			printf("[FAIL]\r\n");
			return;
		}
	}
	printf("[OK]\r\n");

	printf("    Read and check pattern... ");
	if (SD_MMC_OK != sd_mmc_init_read_blocks(slot, last_blocks_addr, TEST_MEM_AREA_SIZE / SD_MMC_BLOCK_SIZE)) {
		printf("Read [FAIL]\r\n");
		return;
	}
	for (nb_trans = 0; nb_trans < (TEST_MEM_AREA_SIZE / TEST_MEM_ACCESS_SIZE); nb_trans++) {
		for (i = 0; i < (TEST_MEM_ACCESS_SIZE / sizeof(uint32_t)); i++) {
			((uint32_t *)buf_test)[i] = 0xFFFFFFFF;
		}
		if (SD_MMC_OK != sd_mmc_start_read_blocks(buf_test, TEST_MEM_ACCESS_SIZE / SD_MMC_BLOCK_SIZE)) {
			printf("Read [FAIL]\r\n");
			return;
		}
		if (SD_MMC_OK != sd_mmc_wait_end_of_read_blocks(false)) {
			printf("Read [FAIL]\r\n");
			return;
		}
		if (((uint32_t *)buf_test)[0] != nb_trans) {
			printf("Check [FAIL]\r\n");
			return;
		}
		for (i = 1; i < (TEST_MEM_ACCESS_SIZE / sizeof(uint32_t)); i++) {
			if (((uint32_t *)buf_test)[i] != TEST_FILL_VALUE_U32) {
				printf("Check [FAIL]\r\n");
				return;
			}
		}
	}
	printf("[OK]\r\n");
}

/**
 * \brief Perform test on CIA (Common I/O Area) of SDIO card.
 *
 * \note The Common I/O Area (CIA) shall be implemented on all SDIO cards.
 *
 * \param slot   SD/MMC slot to test
 */
static void main_test_sdio(uint8_t slot)
{
	uint32_t i;
	uint32_t err;

	/*
	 * Test with direct read and write command.
	 */
	printf("\r\n--- Test with direct read and write command of CIA:\r\n");
	/* Read */
	for (i = 0; i < TEST_CIA_SIZE; i++) {
		err = sdio_read_direct(slot, SDIO_CIA, i, &buf_cia[i]);
		if (err) {
			printf("Error: SDIO direct read failed.\r\n");
			return;
		}
	}
	main_dump_buffer(buf_cia, TEST_CIA_SIZE);

	/* Write */
	printf("Write 0x02 to IEN(CIA.4).\r\n");
	err = sdio_write_direct(slot, SDIO_CIA, SDIO_CCCR_IEN, 0x02);
	if (err) {
		printf("Error: SDIO direct write failed.\r\n");
		return;
	}

	/* Check */
	printf("Check IEN after write:");
	err = sdio_read_direct(slot, SDIO_CIA, SDIO_CCCR_IEN, &buf_cia[SDIO_CCCR_IEN]);
	if (err) {
		printf("Error: SDIO direct read failed.\r\n");
		return;
	}

	printf("0x%02x\r\n", buf_cia[SDIO_CCCR_IEN]);
	if (0x02 == buf_cia[SDIO_CCCR_IEN]) {
		printf("Test OK.\r\n");
	} else {
		printf("Test failed.\r\n");
	}

	/* Restore data to 0 */
	sdio_write_direct(slot, SDIO_CIA, SDIO_CCCR_IEN, 0);

	/* Clear the buffer. */
	memset(buf_cia, 0x0, sizeof(buf_cia));

	printf("\r\n--- Test with extended read and write command of CIA:\r\n");
	/* Read */
	err = sdio_read_extended(slot, SDIO_CIA, 0, 1, &buf_cia[0], TEST_CIA_SIZE);
	if (err) {
		printf("Error: SDIO extended read failed.\r\n");
		return;
	}

	main_dump_buffer(buf_cia, TEST_CIA_SIZE);

	/* Write */
	printf("Modify Some R/W bytes for FN0 and write:\r\n");
	buf_cia[SDIO_CCCR_IEN] = 0x3;
	err                    = sdio_write_extended(slot, SDIO_CIA, SDIO_CCCR_IEN, 1, &buf_cia[SDIO_CCCR_IEN], 1);
	if (err) {
		printf("Error: SDIO extended write failed.\r\n");
		return;
	}

	/* Read and check */
	printf("Check CIA after write:\r\n");
	err = sdio_read_extended(slot, SDIO_CIA, 0, 1, &buf_cia[0], TEST_CIA_SIZE);
	if (err) {
		printf("Error: SDIO extended read failed.\r\n");
		return;
	}

	main_dump_buffer(buf_cia, TEST_CIA_SIZE);

	if (buf_cia[SDIO_CCCR_IEN] != 0x3) {
		printf("CIA.4 Fail\r\n");
	} else {
		printf("Test OK\r\n");
	}

	/* Restore data to 0 */
	sdio_write_direct(slot, SDIO_CIA, SDIO_CCCR_IEN, 0);

	return;
}

/**
 * \brief Display basic information of the card.
 * \note This function should be called only after the card has been
 *       initialized successfully.
 *
 * \param slot   SD/MMC slot to test
 */
static void main_display_info_card(uint8_t slot)
{
	printf("Card information:\r\n");

	printf("    ");
	switch (sd_mmc_get_type(slot)) {
	case CARD_TYPE_SD | CARD_TYPE_HC:
		printf("SDHC");
		break;
	case CARD_TYPE_SD:
		printf("SD");
		break;
	case CARD_TYPE_MMC | CARD_TYPE_HC:
		printf("MMC High Density");
		break;
	case CARD_TYPE_MMC:
		printf("MMC");
		break;
	case CARD_TYPE_SDIO:
		printf("SDIO\r\n");
		return;
	case CARD_TYPE_SD_COMBO:
		printf("SD COMBO");
		break;
	case CARD_TYPE_UNKNOWN:
	default:
		printf("Unknow\r\n");
		return;
	}
	printf("\r\n    %d MB\r\n", (uint16_t)(sd_mmc_get_capacity(slot) / 1024));
}

void sdtester(void)
{
	sd_mmc_err_t err;

	printf("Please plug an SD, MMC or SDIO card in slot.\r\n");

	/* Wait for a card and ready */
	do {
		err = sd_mmc_check(0);
	} while (SD_MMC_OK != err);

	/* Display basic card information */
	main_display_info_card(0);

	/* Test the card */
	if (sd_mmc_get_type(0) & CARD_TYPE_SDIO) {
		/* Test CIA of SDIO card */
		main_test_sdio(0);
	}

	if (sd_mmc_get_type(0) & (CARD_TYPE_SD | CARD_TYPE_MMC)) {
		/* SD/MMC Card R/W */
		main_test_memory(0);
	}

	printf("Test finished, please unplugged the card.\r\n");

}