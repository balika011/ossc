//
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
#include "system.h"
#include "osd.h"
#include "flash.h"
#include "controls.h"
#include "sdcard.h"
#include "utils.h"

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

static int check_fw_header(fw_hdr *hdr)
{
	if (hdr->params.magic != __builtin_bswap32('OSSC'))
		return FW_IMAGE_ERROR;

	if (__builtin_bswap32(hdr->params.hdr_len) < 26 || __builtin_bswap32(hdr->params.hdr_len) > 508)
		return FW_HDR_ERROR;

	if (crc32(0, (uint8_t *)hdr, __builtin_bswap32(hdr->params.hdr_len)) != __builtin_bswap32(hdr->raw.hdr_crc))
		return FW_HDR_CRC_ERROR;

	hdr->params.data_len = __builtin_bswap32(hdr->params.data_len);
	hdr->params.data_crc = __builtin_bswap32(hdr->params.data_crc);

	return 0;
}

int fw_update()
{
	int retval = sdcard_check();
	if (retval != 0)
	{
		retval = -retval;
		goto failure;
	}

	fw_hdr fw_header;
	retval = sdcard_read(0, (uint8_t *)&fw_header, sizeof(fw_header));
	if (retval != 0)
	{
		retval = -retval;
		goto failure;
	}

	retval = check_fw_header(&fw_header);
	if (retval != 0)
		goto failure;

	char row[LCD_ROW_LEN + 1];
	sniprintf(row, sizeof(row), "%u bytes", fw_header.params.data_len);
	osd_notification("Validating data", row);

	retval = sdcard_check_crc(sizeof(fw_hdr), fw_header.params.data_len, fw_header.params.data_crc);
	if (retval != 0)
		goto failure;

	sniprintf(row, sizeof(row), "%u.%.2u%s%s", fw_header.params.version_major, fw_header.params.version_minor, (fw_header.params.version_suffix[0] == 0) ? "" : "-", fw_header.params.version_suffix);
	osd_notification(row, "Update? 1=Y, 2=N");

	while (1)
	{
		controls_update();

		if (remote_code == rc_keymap[RC_BTN1])
		{
			break;
		}
		else if (remote_code == rc_keymap[RC_BTN2])
		{
			retval = FW_UPD_CANCELLED;
			goto failure;
		}

		usleep(10000);
	}

	osd_deinit();
	tvp_powerdown();
	SC->sys_ctrl.enable_sc = 1;

	usleep(10000);

	// copy the updater into the frame buffer then jump to it
	memcpy(OSDFB8, (void *) (FLASH_MEM_BASE + FLASH_UPDATER_OFFSET), 0x3000);
	((void (*)(fw_hdr *))(OSDFB8 + 0x80))(&fw_header);

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

	return -retval;
}
