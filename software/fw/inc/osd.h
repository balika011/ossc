//
// Copyright (C) 2015-2016  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#ifndef OSD_H_
#define OSD_H_

#include <stdint.h>
#include "lcd.h"
#include "video_modes.h"

extern uint8_t osd_enable, osd_status_timeout;

void osd_init();
void osd_deinit();
void osd_notification(const char *row1, const char *row2);
void osd_status(const char *row1, const char *row2);
void osd_set_menu_active(char active);
void osd_update_size(mode_data_t *vm_out);

void osd_clear();
void osd_draw_char(uint16_t x, uint16_t y, uint8_t bg, uint8_t fg, char chr);
void osd_draw_text(uint8_t row, uint8_t column, uint8_t bg, uint8_t fg, const char *text);

#endif