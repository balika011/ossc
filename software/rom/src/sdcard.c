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

#include <io.h>
#include <string.h>
#include "sdcard.h"
#include "flash.h"
#include "utils.h"
#include "mmc.h"

extern struct mmc *mmc_dev;

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
	if (mmc_bread(mmc_dev, offset / SD_BLK_SIZE, (len + SD_BLK_SIZE - 1) / SD_BLK_SIZE, buf) != (len + SD_BLK_SIZE - 1) / SD_BLK_SIZE)
	{
		printf("Failed to read SD card\n");
		return -1;
	}

	return 0;
}

int sdcard_check_crc(uint32_t offset, uint32_t size, uint32_t golden_crc)
{
	uint8_t temp[SD_BLK_SIZE];
	alt_u32 crcval = 0, i, bytes_to_read;

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

int copy_flash_to_sd(alt_u32 flash_pagenum, alt_u32 sd_blknum, alt_u32 length, alt_u8 *tmpbuf, alt_u8 swap_bits)
{
#if 0
    int retval, i;
    alt_u32 bytes_to_rw;

    while (length > 0) {
        bytes_to_rw = (length < SD_BLK_SIZE) ? length : SD_BLK_SIZE;
        retval = alt_epcq_controller2_read(epcq_dev, flash_pagenum*PAGESIZE, tmpbuf, bytes_to_rw);

		if (swap_bits)
		{
			for (i=0; i<bytes_to_rw; i++)
				tmpbuf[i] = bitswap8(tmpbuf[i]);
		}
		if (retval != 0)
            return retval;

        if (bytes_to_rw < SD_BLK_SIZE)
            memset(tmpbuf+bytes_to_rw, 0, SD_BLK_SIZE-bytes_to_rw);

		if (mmc_bwrite(mmc_dev, sd_blknum, 1, tmpbuf) != 1) {
            printf("Failed to write to SD card\n");
            return -1;
        }

        ++sd_blknum;
        flash_pagenum += bytes_to_rw/PAGESIZE;
        length -= bytes_to_rw;
	}
#endif

	return 0;
}
