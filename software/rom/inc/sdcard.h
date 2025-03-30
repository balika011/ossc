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

#ifndef SDCARD_H_
#define SDCARD_H_

#include <stdint.h>
#include "alt_types.h"
#include "sysconfig.h"

#define SD_BLK_SIZE 512

#define SD_NOINIT                99
#define FW_IMAGE_ERROR          100
#define FW_HDR_ERROR            101
#define FW_HDR_CRC_ERROR        102
#define FW_DATA_CRC_ERROR       103
#define FW_UPD_CANCELLED        104

int sdcard_check();
int sdcard_read(uint32_t offset, uint8_t *buf, uint32_t len);
int sdcard_check_crc(uint32_t offset, uint32_t size, uint32_t golden_crc);

int copy_flash_to_sd(alt_u32 flash_pagenum, alt_u32 sd_blknum, alt_u32 length, alt_u8 *tmpbuf, alt_u8 swap_bits);

#endif /* SDCARD_H_ */
