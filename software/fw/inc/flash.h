//
// Copyright (C) 2015-2016  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#ifndef FLASH_H_
#define FLASH_H_

#include <stdint.h>
#include "sysconfig.h"

#define FLASH_RBF_OFFSET 0
#define FLASH_FW_OFFSET 0x80000
#define FLASH_USER_OFFSET 0x100000

#define USERDATA_OFFSET FLASH_USER_OFFSET
#define MAX_USERDATA_ENTRY 15 // 16 sectors for userdata

#define FLASH_VERIFY_ERROR      204

void flash_write_protect(int enable);
void flash_erase_4k(uint32_t addr);
void flash_erase_32k(uint32_t addr);
void flash_erase_64k(uint32_t addr);

#endif /* FLASH_H_ */
