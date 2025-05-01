//
// Copyright (C) 2015-2023  Markus Hiienkari <mhiienka@niksula.hut.fi>
// Copyright (C) 2025       Balázs Triszka   <info@balika011.hu>
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

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <system.h>
#include <i2c_opencores.h>
#include <lcd.h>

#include "sdcard.h"
#include "flash.h"
#include "utils.h"

#define FW_UPDATE_RETRIES 3

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
		uint32_t data_len;
		uint32_t data_crc;
	} params;
	struct
	{
		uint8_t buf[HDR_SIZE - 4];
		uint32_t hdr_crc;
	} raw;
	/* data */
} fw_hdr;

#pragma pack(pop)

int main(fw_hdr *fw_header)
{
	I2C_init(I2CA_BASE, ALT_CPU_FREQ, 400000);

	lcd_init();

	sdcard_init();

	int retval = sdcard_check();
	if (retval != 0) {
		retval = -retval;
		goto failure;
	}

	int retries = FW_UPDATE_RETRIES;

update_init:

	flash_write_protect(0);

	lcd_write_row1("Eraseing flash");
	lcd_write_row2("please wait...");

	for (int i = 0; i < FLASH_USER_OFFSET; i += 0x10000)
		flash_erase_64k(i);

	lcd_write_row1("Updating FW");

	for (int i = 0; i < fw_header->params.data_len; i += SD_BLK_SIZE)
	{
		uint8_t databuf[SD_BLK_SIZE];
		retval = sdcard_read(sizeof(fw_hdr) + i, databuf, SD_BLK_SIZE);
		if (retval != 0)
			goto failure;

		for (int j = 0; j < SD_BLK_SIZE; j += 4)
			*(uint32_t *)(FLASH_MEM_BASE + i + j) = *(uint32_t *) &databuf[j];
	}

	flash_write_protect(1);

	lcd_write_row1("Verifying flash");

	if (crc32(0, (void *)FLASH_MEM_BASE, fw_header->params.data_len) != fw_header->params.data_crc)
	{
		retval = FLASH_VERIFY_ERROR;
		goto failure;
	}

	lcd_write_row1("Firmware updated");
	lcd_write_row2("Restart in 3 sec");

	usleep(3000000);

	SC->sys_ctrl.hw_reset = 1;
	while (1);

failure:
	char *errmsg = "SD/Flash error";
	switch (retval) {
		case SD_NOINIT: errmsg = "No SD card det."; break;
		case FW_IMAGE_ERROR: errmsg = "Invalid image"; break;
		case FW_HDR_ERROR: errmsg = "Invalid header"; break;
		case FW_HDR_CRC_ERROR: errmsg = "Invalid hdr CRC"; break;
		case FW_DATA_CRC_ERROR: errmsg = "Invalid data CRC"; break;
		case FW_UPD_CANCELLED: errmsg = "Update cancelled"; break;
		case FLASH_VERIFY_ERROR: errmsg = "Flash verif fail"; break;
	}

	lcd_write_row1("Update failed");
	lcd_write_row2(errmsg);

	usleep(1000000);

	// Critical error, retry update
	if (retval < 0 && retries > 0) {
		retries--;
		goto update_init;
	}

	SC->sys_ctrl.hw_reset = 1;
	while(1);
}