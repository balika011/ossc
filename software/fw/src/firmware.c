//
// Copyright (C) 2015-2023  Markus Hiienkari <mhiienka@niksula.hut.fi>
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
#include "system.h"
#include "firmware.h"
#include "sdcard.h"
#include "flash.h"
#include "controls.h"
#include "tvp7002.h"
#include "av_controller.h"
#include "lcd.h"
#include "utils.h"
#include "menu.h"

#define FW_UPDATE_RETRIES 3

typedef struct
{
	char fw_key[4];
	uint8_t version_major;
	uint8_t version_minor;
	char version_suffix[8];
	uint32_t hdr_len;
	uint32_t data_len;
	uint32_t data_crc;
	uint32_t hdr_crc;
} fw_hdr;

static int check_fw_header(uint8_t *databuf, fw_hdr *hdr)
{
	uint32_t tmp;

	strncpy(hdr->fw_key, (char*)databuf, 4);
	if (strncmp(hdr->fw_key, "OSSC", 4))
		return FW_IMAGE_ERROR;

	hdr->version_major = databuf[4];
	hdr->version_minor = databuf[5];
	strncpy(hdr->version_suffix, (char*)(databuf+6), 8);
	hdr->version_suffix[7] = 0;

	memcpy(&tmp, databuf+14, 4);
	hdr->hdr_len = __builtin_bswap32(tmp);
	memcpy(&tmp, databuf+18, 4);
	hdr->data_len = __builtin_bswap32(tmp);
	memcpy(&tmp, databuf+22, 4);
	hdr->data_crc = __builtin_bswap32(tmp);
	// Always at bytes [508-511]
	memcpy(&tmp, databuf+508, 4);
	hdr->hdr_crc = __builtin_bswap32(tmp);

	if (hdr->hdr_len < 26 || hdr->hdr_len > 508)
		return FW_HDR_ERROR;

	if (crc32(0, databuf, hdr->hdr_len) != hdr->hdr_crc)
		return FW_HDR_CRC_ERROR;

	return 0;
}

static int check_fw_image(uint32_t offset, uint32_t size, uint32_t golden_crc, uint8_t *tmpbuf)
{
	uint32_t crcval = 0, i, bytes_to_read;

	for (uint32_t i = 0; i < size; i = i + SD_BLK_SIZE)
	{
		uint32_t bytes_to_read = ((size - i < SD_BLK_SIZE) ? (size - i) : SD_BLK_SIZE);

		int res = sdcard_read(offset + i, tmpbuf, bytes_to_read);
		if (res != 0)
			return -res;

		crcval = crc32(crcval, tmpbuf, bytes_to_read);
	}

	if (crcval != golden_crc)
		return FW_DATA_CRC_ERROR;

	return 0;
}

int fw_update()
{
	int retval = sdcard_check();
	if (retval != 0) {
		retval = -retval;
		goto failure;
	}

	uint8_t databuf[SD_BLK_SIZE];
	retval = sdcard_read(0, databuf, SD_BLK_SIZE);
	if (retval != 0)
		goto failure;

	sniprintf(menu_row1, LCD_ROW_LEN + 1, "%x %x %x %x", databuf[0], databuf[1], databuf[2], databuf[3]);
	sniprintf(menu_row2, LCD_ROW_LEN + 1, "");
	ui_disp_menu(1);

	fw_hdr fw_header;
	retval = check_fw_header(databuf, &fw_header);
	if (retval != 0)
		goto failure;

	sniprintf(menu_row1, LCD_ROW_LEN+1, "Validating data");
	sniprintf(menu_row2, LCD_ROW_LEN+1, "%u bytes", (unsigned)fw_header.data_len);
	ui_disp_menu(1);
	retval = check_fw_image(512, fw_header.data_len, fw_header.data_crc, databuf);
	if (retval != 0)
		goto failure;

	sniprintf(menu_row1, LCD_ROW_LEN+1, "%u.%.2u%s%s", fw_header.version_major, fw_header.version_minor, (fw_header.version_suffix[0] == 0) ? "" : "-", fw_header.version_suffix);
	strncpy(menu_row2, "Update? 1=Y, 2=N", LCD_ROW_LEN+1);
	ui_disp_menu(1);

	while (1) {
		uint32_t btn_vec = SC->controls.ir_code;

		if (btn_vec == rc_keymap[RC_BTN1]) {
			break;
		} else if (btn_vec == rc_keymap[RC_BTN2]) {
			retval = FW_UPD_CANCELLED;
			goto failure;
		}

		usleep(WAITLOOP_SLEEP_US);
	}

	//disable video output
	tvp_powerdown();
	SC->sys_ctrl.enable_sc = 1;
	usleep(10000);

	strncpy(menu_row1, "Updating FW", LCD_ROW_LEN + 1);

	int retries = FW_UPDATE_RETRIES;

update_init:
	strncpy(menu_row2, "please wait...", LCD_ROW_LEN+1);
	ui_disp_menu(1);

	for (int i = 0; i < fw_header.data_len; i += SD_BLK_SIZE)
	{
		retval = sdcard_read(512 + i, databuf, SD_BLK_SIZE);
		if (retval != 0)
			goto failure;

		for (int j = 0; j < SD_BLK_SIZE; j += 4)
			*(uint32_t *)(FLASH_MEM_BASE + i + j) = *(uint32_t *) &databuf[j];
	}

	strncpy(menu_row1, "Verifying flash", LCD_ROW_LEN+1);
	ui_disp_menu(1);

	if (crc32(0, (void *) FLASH_MEM_BASE, fw_header.data_len) != fw_header.data_crc)
	{
		retval = FLASH_VERIFY_ERROR;
		goto failure;
	}

	strncpy(menu_row1, "Firmware updated", LCD_ROW_LEN+1);
	strncpy(menu_row2, "", LCD_ROW_LEN+1);
	ui_disp_menu(1);

	SC->sys_ctrl.hw_reset = 1;

	return 0;

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
	strncpy(menu_row2, errmsg, LCD_ROW_LEN+1);
	ui_disp_menu(1);
	usleep(1000000);

	// Critical error, retry update
	if ((retval < 0) && (retries > 0)) {
		sniprintf(menu_row1, LCD_ROW_LEN+1, "Retrying update");
		retries--;
		goto update_init;
	}

	render_osd_page();
	return -1;
}
