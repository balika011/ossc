//
// Copyright (C) 2025  Balázs Triszka <info@balika011.hu>
//
// This file is part of Open Source Scan Converter project.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "system.h"
#include "alt_types.h"
#include "sys/alt_timestamp.h"
#include "lcd.h"
#include "altera_avalon_pio_regs.h"
#include "i2c_opencores.h"
#include "utils.h"
#include "flash.h"
#include "sdcard.h"
#include "mmc.h"

struct mmc *ocsdc_mmc_init(int base_addr, int clk_freq, unsigned int host_caps);
struct mmc *mmc_dev;
#define SDC_FREQ 108000000U
#define SDC_HOST_CAPS (MMC_MODE_HS | MMC_MODE_HS_52MHz | MMC_MODE_4BIT)

char row1[LCD_ROW_LEN + 1], row2[LCD_ROW_LEN + 1];

// Initialize hardware
int init_hw()
{
	// Start system timer
	alt_timestamp_start();

	// Reset hardware
	SC->sys_ctrl.led_g = 1;
	SC->sys_ctrl.lcd_bl_on = 1;
	SC->sys_ctrl.pll_bypass = 1;
	SC->sys_ctrl.lcd_cs_n = 1;

#ifndef HAS_SH1107
	if (SC->controls.is_1v8))
		SC->sys_ctrl.remap_red_r = 1;
#else
	SC->sys_ctrl.remap_lcd_bl = 1;
#endif

	SC->hv_in_config.data = 0;
	SC->hv_in_config2.data = 0;
	SC->hv_in_config3.data = 0;
	usleep(10000);

	// unreset hw
	SC->sys_ctrl.av_reset = 1;

	//wait >500ms for SD card interface to be stable
    //over 200ms and LCD may be buggy?
	usleep(200000);

	I2C_init(I2CA_BASE, ALT_CPU_FREQ, 400000);

	mmc_dev = ocsdc_mmc_init(SDC_CONTROLLER_BASE, SDC_FREQ, SDC_HOST_CAPS);

	/* Initialize the character display */
    lcd_init();

	return 0;
}

#define HDR_SIZE 512
#define FW_SUFFIX_MAX_SIZE 8

#pragma pack(push, 1)

typedef union
{
	struct
	{
		uint32_t magic;
		uint8_t version_major;
		uint8_t version_minor;
		char version_suffix[FW_SUFFIX_MAX_SIZE];
		uint32_t hdr_len;
		uint32_t rbf_len;
		uint32_t rbf_crc;
		uint32_t fw_len;
		uint32_t fw_crc;
	} params;
	struct
	{
		uint8_t buf[HDR_SIZE - 4];
		uint32_t hdr_crc;
	} raw;
	/* data */
} fw_hdr;

#pragma pack(pop)

static int check_fw_header(fw_hdr *hdr)
{
	if (hdr->params.magic != bswap32('OSSC'))
		return FW_IMAGE_ERROR;

	if (bswap32(hdr->params.hdr_len) < 26 || bswap32(hdr->params.hdr_len) > 508)
		return FW_HDR_ERROR;

	if (crc32(0, (uint8_t *)hdr, bswap32(hdr->params.hdr_len)) != bswap32(hdr->raw.hdr_crc))
		return FW_HDR_CRC_ERROR;

	hdr->params.rbf_len = bswap32(hdr->params.rbf_len);
	hdr->params.rbf_crc = bswap32(hdr->params.rbf_crc);
	hdr->params.fw_len = bswap32(hdr->params.fw_len);
	hdr->params.fw_crc = bswap32(hdr->params.fw_crc);

	return 0;
}

int main()
{
	printf("Hi\n");

	int init_stat = init_hw();
	if (init_stat < 0)
	{
		sniprintf(row1, sizeof(row1), "Init error  %d", init_stat);
		row2[0] = 0;
		lcd_write(row1, row2);
		while (1);
	}

	int enter_dfu = SC->controls.btn1;

	strcpy(row1, "      OSSC");
	sniprintf(row2, sizeof(row2), "      %u.%.2u" FW_SUFFIX, FW_VER_MAJOR, FW_VER_MINOR);
	lcd_write(row1, row2);
	usleep(500000);

	if (!enter_dfu)
	{
		uint32_t crc = crc32(0, (uint8_t *)(FLASH_MEM_BASE + FLASH_FW_OFFSET), 0x7FFFC);
		if (crc == *(uint32_t *)(FLASH_MEM_BASE + FLASH_FW_OFFSET + 0x7FFFC))
		{
			printf("Bootin'\n");

			((void (*)()) (FLASH_MEM_BASE + FLASH_FW_OFFSET + 0x80))();

			while (1);
		}
	}

	strcpy(row1, "    DFU MODE");
	strcpy(row2, "Detecting...");
	lcd_write(row1, row2);

	int retval = sdcard_check();
	if (retval != 0)
	{
		retval = -retval;
		goto failure;
	}

	strcpy(row2, "Reading...");
	lcd_write(row1, row2);

	fw_hdr fw_header;
	retval = sdcard_read(0, (uint8_t *)&fw_header, sizeof(fw_header));
	if (retval != 0)
	{
		retval = -retval;
		goto failure;
	}

	strcpy(row2, "Validating...");
	lcd_write(row1, row2);

	retval = check_fw_header(&fw_header);
	if (retval != 0)
		goto failure;

	retval = sdcard_check_crc(sizeof(fw_hdr), fw_header.params.rbf_len, fw_header.params.rbf_crc);
	if (retval != 0)
		goto failure;

	if (fw_header.params.fw_len)
	{
		retval = sdcard_check_crc((sizeof(fw_hdr) + fw_header.params.rbf_len + SD_BLK_SIZE - 1) & ~0x1ff, fw_header.params.fw_len, fw_header.params.fw_crc);
		if (retval != 0)
			goto failure;
	}

	sniprintf(row1, sizeof(row1), "%u.%.2u%s%s", fw_header.params.version_major, fw_header.params.version_minor, (fw_header.params.version_suffix[0] == 0) ? "" : "-", fw_header.params.version_suffix);
	strncpy(row2, "Update? BTN2=Y", sizeof(row2));
	lcd_write(row1, row2);

	while (SC->controls.btn1 || SC->controls.btn2);

	usleep(10000);

	while (1)
	{
		if (SC->controls.btn1)
		{
			retval = FW_UPD_CANCELLED;
			goto failure;
		}

		if (SC->controls.btn2)
			break;

		usleep(WAITLOOP_SLEEP_US);
	}

	strncpy(row1, "Updating FW", sizeof(row1));
	strncpy(row2, "please wait...", sizeof(row2));
	lcd_write(row1, row2);

	flash_write_protect(0);

	for (int i = 0; i < 0x80000; i += 0x10000)
		flash_erase_64k(FLASH_RBF_OFFSET + i);

	if (fw_header.params.fw_len)
	{
		for (int i = 0; i < 0x80000; i += 0x10000)
			flash_erase_64k(FLASH_FW_OFFSET + i);
	}

	uint8_t temp[SD_BLK_SIZE];
	for (int i = 0; i < fw_header.params.rbf_len; i += SD_BLK_SIZE)
	{
		retval = sdcard_read(sizeof(fw_hdr) + i, temp, SD_BLK_SIZE);
		if (retval != 0)
			goto failure;

		for (int j = 0; j < SD_BLK_SIZE; j++)
			temp[j] = bitswap8(temp[j]);

		for (int j = 0; j < SD_BLK_SIZE; j += 4)
			*(uint32_t *)(FLASH_MEM_BASE + FLASH_RBF_OFFSET + i + j) = *(uint32_t *)&temp[j];
	}

	for (int i = 0; i < fw_header.params.fw_len; i += SD_BLK_SIZE)
	{
		retval = sdcard_read(sizeof(fw_hdr) + fw_header.params.rbf_len + SD_BLK_SIZE - 1 + i, temp, SD_BLK_SIZE);
		if (retval != 0)
			goto failure;

		for (int j = 0; j < SD_BLK_SIZE; j += 4)
			*(uint32_t *)(FLASH_MEM_BASE + FLASH_FW_OFFSET + i + j) = *(uint32_t *)&temp[j];
	}

	strncpy(row1, "Verifying flash", sizeof(row1));
	lcd_write(row1, row2);
	retval = flash_verify(FLASH_RBF_OFFSET, fw_header.params.rbf_len, fw_header.params.rbf_crc, 1);
	if (retval != 0)
		goto failure;

	if (fw_header.params.fw_len)
	{
		retval = flash_verify(FLASH_FW_OFFSET, fw_header.params.fw_len, fw_header.params.fw_crc, 0);
		if (retval != 0)
			goto failure;
	}

	*(uint32_t *)(FLASH_MEM_BASE + FLASH_FW_OFFSET + 0x7FFFC) = crc32(0, (uint8_t *)(FLASH_MEM_BASE + FLASH_FW_OFFSET), 0x7FFFC);

	flash_write_protect(1);

	strncpy(row1, "Firmware updated", sizeof(row1));
	strncpy(row2, "Restart in 5 sec", sizeof(row2));
	lcd_write(row1, row2);

	usleep(5000000);

	SC->sys_ctrl.hw_reset = 1;

	while (1);

failure:
	char *errmsg;
	switch (retval)
	{
	case SD_NOINIT:
		errmsg = "No card deteced";
		break;
	case FW_IMAGE_ERROR:
		errmsg = " Invalid image";
		break;
	case FW_HDR_ERROR:
		errmsg = " Invalid header";
		break;
	case FW_HDR_CRC_ERROR:
		errmsg = "Invalid hdr CRC";
		break;
	case FW_DATA_CRC_ERROR:
		errmsg = "Invalid data CRC";
		break;
	case FW_UPD_CANCELLED:
		errmsg = "Update cancelled";
		break;
	case FLASH_VERIFY_ERROR:
		errmsg = "Failed to verify";
		break;
	default:
		errmsg = "SD/Flash error";
		break;
	}
	strncpy(row2, errmsg, sizeof(row2));
	lcd_write(row1, row2);

	usleep(3000000);

	SC->sys_ctrl.hw_reset = 1;

	while(1);

    return 0;
}
