//
// Copyright (C) 2015-2016  Markus Hiienkari <mhiienka@niksula.hut.fi>
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
#include "system.h"
#include "sdcard.h"
#include "flash.h"
#include "utils.h"
#include "mmc.h"

struct mmc *ocsdc_mmc_init(int base_addr, int clk_freq, unsigned int host_caps);
struct mmc *mmc_dev;
#define SDC_FREQ 108000000
#define SDC_HOST_CAPS (MMC_MODE_HS | MMC_MODE_HS_52MHz | MMC_MODE_4BIT)

void sdcard_init()
{
	mmc_dev = ocsdc_mmc_init(SDC_CONTROLLER_BASE, SDC_FREQ, SDC_HOST_CAPS);
}

int sdcard_check()
{
	int res = mmc_init(mmc_dev);
	if (res != 0 || mmc_dev->has_init == 0)
		return -SD_NOINIT;

	printf("SD det status: %u\n", res);

	return 0;
}

int sdcard_read(uint32_t offset, uint8_t *buf, uint32_t len)
{
	uint32_t full_block_len = (len / SD_BLK_SIZE) * SD_BLK_SIZE;

	if (full_block_len)
	{
		if (mmc_bread(mmc_dev, offset / SD_BLK_SIZE, full_block_len / SD_BLK_SIZE, buf) != full_block_len / SD_BLK_SIZE)
		{
			printf("Failed to read SD card\n");
			return -1;
		}
	}

	if (len != full_block_len)
	{
		uint8_t temp[SD_BLK_SIZE];

		if (mmc_bread(mmc_dev, (offset + full_block_len) / SD_BLK_SIZE, 1, temp) != 1)
		{
			printf("Failed to read SD card\n");
			return -2;
		}

		memcpy(&buf[full_block_len], temp, len - full_block_len);
	}

	return 0;
}

int sdcard_write(uint32_t offset, uint8_t *buf, uint32_t len)
{
	if (mmc_bwrite(mmc_dev, offset / SD_BLK_SIZE, (len + SD_BLK_SIZE - 1) / SD_BLK_SIZE, buf) != (len + SD_BLK_SIZE - 1) / SD_BLK_SIZE)
	{
		printf("Failed to write SD card\n");
		return -1;
	}

	return 0;
}

int sdcard_check_crc(uint32_t offset, uint32_t size, uint32_t golden_crc)
{
	uint8_t temp[SD_BLK_SIZE];
	uint32_t crcval = 0, i, bytes_to_read;

	for (i = 0; i < size; i += SD_BLK_SIZE)
	{
		bytes_to_read = ((size - i < SD_BLK_SIZE) ? (size - i) : SD_BLK_SIZE);
		if (mmc_bread(mmc_dev, (offset + i) / SD_BLK_SIZE, 1, temp) != 1)
		{
			return -1;
		}

		crcval = crc32(crcval, temp, bytes_to_read);
	}

	if (crcval != golden_crc)
		return FW_DATA_CRC_ERROR;

	return 0;
}