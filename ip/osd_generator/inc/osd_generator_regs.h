//
// Copyright (C) 2019-2020  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#ifndef OSD_GENERATOR_REGS_H_
#define OSD_GENERATOR_REGS_H_

#define OSD_CHAR_ROWS 30
#define OSD_CHAR_COLS 16
#define OSD_CHAR_SECTIONS 2

#include <stdint.h>

typedef struct
{
	volatile uint32_t enable;
	volatile uint16_t xpos;
	volatile uint16_t ypos;
	volatile uint16_t xsize;
	volatile uint16_t ysize;
	volatile uint32_t colors[4];
} __attribute__((packed)) osd_regs;

#endif //OSD_GENERATOR_REGS_H_
