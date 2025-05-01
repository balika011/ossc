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

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "system.h"
#include "osd.h"
#include "menu.h"
#include "timer.h"

#include "osd_ibm_vga_font.h"

#define RENDER_SPEEDUP

uint8_t osd_enable = 1, osd_status_timeout = 1;

char osd_menu_active = 0;

static int timer_idx = -1;

#define OSC_WIDTH 128
#define OSC_HEIGHT 384

int osd_notification_y = 0;

#ifdef RENDER_SPEEDUP
uint16_t remap_norm[256];
uint16_t remap_highlight[256];
#endif

void osd_init()
{
	OSD->colors[0] = 0x00000000;
	OSD->colors[1] = 0x7f000000;
	OSD->colors[2] = 0xffffffff;
	OSD->colors[3] = 0xffffff00;

#ifdef RENDER_SPEEDUP
	for (int i = 0; i < 256; i++)
	{
		remap_norm[i] =
			((((i >> 0) & 1) ? 2 : 1) << (7 * 2)) |
			((((i >> 1) & 1) ? 2 : 1) << (6 * 2)) |
			((((i >> 2) & 1) ? 2 : 1) << (5 * 2)) |
			((((i >> 3) & 1) ? 2 : 1) << (4 * 2)) |
			((((i >> 4) & 1) ? 2 : 1) << (3 * 2)) |
			((((i >> 5) & 1) ? 2 : 1) << (2 * 2)) |
			((((i >> 6) & 1) ? 2 : 1) << (1 * 2)) |
			((((i >> 7) & 1) ? 2 : 1) << (0 * 2));
		remap_highlight[i] =
			((((i >> 0) & 1) ? 3 : 1) << (7 * 2)) |
			((((i >> 1) & 1) ? 3 : 1) << (6 * 2)) |
			((((i >> 2) & 1) ? 3 : 1) << (5 * 2)) |
			((((i >> 3) & 1) ? 3 : 1) << (4 * 2)) |
			((((i >> 4) & 1) ? 3 : 1) << (3 * 2)) |
			((((i >> 5) & 1) ? 3 : 1) << (2 * 2)) |
			((((i >> 6) & 1) ? 3 : 1) << (1 * 2)) |
			((((i >> 7) & 1) ? 3 : 1) << (0 * 2));
	}
#endif
}

void osd_deinit()
{
	if (timer_idx >= 0)
	{
		timer_cancel(timer_idx);
		timer_idx = -1;
	}
	osd_enable = 0;
	OSD->enable = 0;
}

void osd_notification(const char *row1, const char *row2)
{
	OSD->ysize = OSC_WIDTH;
	OSD->ypos = osd_notification_y;
	osd_clear();
	osd_draw_text(0, 0, 1, 2, row1);
	if (row2[0])
		osd_draw_text(1, 0, 1, 2, row2);

	lcd_write_row1(row1);
	lcd_write_row2(row2);
}

static void osd_off()
{
	OSD->enable = 0;
}

char osd_status_text[2][OSD_CHAR_COLS + 1];

void osd_status(const char *row1, const char *row2)
{
	strncpy(osd_status_text[0], row1, sizeof(osd_status_text[0]));
	strncpy(osd_status_text[1], row2, sizeof(osd_status_text[1]));

	if (!osd_menu_active)
	{
		osd_notification(row1, row2);

		if (!osd_enable)
		{
			OSD->enable = 0;
			return;
		}

		if (timer_idx >= 0)
		{
			timer_cancel(timer_idx);
			timer_idx = -1;
		}

		OSD->enable = 1;

		if (osd_status_timeout)
		{
			int timeout = 0;
			switch (osd_status_timeout)
			{
			case 0:
				timeout = 2000000;
				break;
			case 1:
				timeout = 5000000;
				break;
			case 2:
				timeout = 10000000;
				break;
			}

			timer_idx = timer_timeout(timeout, osd_off);
		}
	}
}

void osd_set_menu_active(char active)
{
	osd_menu_active = active;

	if (!osd_menu_active)
	{
		lcd_write_row1(osd_status_text[0]);
		lcd_write_row2(osd_status_text[1]);
	}

	if (!osd_enable)
	{
		OSD->enable = 0;
		return;
	}

	if (osd_menu_active)
	{
		if (timer_idx >= 0)
		{
			timer_cancel(timer_idx);
			timer_idx = -1;
		}

		OSD->ysize = OSC_WIDTH * 2;
		OSD->ypos = 10;
		OSD->enable = 1;
	}
	else
	{
		if (timer_idx == -1)
			OSD->enable = 0;
	}
}

void osd_update_size(mode_data_t *vm_out)
{
#if 0
	uint8_t osd_size = vm_out->timings.v_active / 700;
	uint8_t par_x4 = (((400 * vm_out->timings.h_active * vm_out->ar.v) / ((vm_out->timings.v_active << vm_out->timings.interlaced) * vm_out->ar.h)) + 50) / 100;
	int8_t xadj_log2 = -2;

	while (par_x4 > 1)
	{
		par_x4 >>= 1;
		xadj_log2++;
	}

	OSD->osd_config.x_size = ((osd_size + vm_out->timings.interlaced + xadj_log2) >= 0) ? (osd_size + vm_out->timings.interlaced + xadj_log2) : 0;
	OSD->osd_config.y_size = osd_size;
#endif

	osd_notification_y = vm_out->timings.h_active - OSC_WIDTH - 10;

	OSD->xsize = OSC_HEIGHT;
	OSD->ysize = osd_menu_active ? (OSC_WIDTH * 2) : OSC_WIDTH;
	OSD->xpos = 10;
	OSD->ypos = osd_menu_active ? 10 : osd_notification_y;
}

void osd_clear()
{
	OSD->ysize = osd_menu_active ? (OSC_WIDTH * 2) : OSC_WIDTH;
	OSD->ypos = osd_menu_active ? 10 : osd_notification_y;

	memset(OSDFB8, 0x00, 0x6000);
}

void osd_draw_char(uint16_t x, uint16_t y, uint8_t bg, uint8_t fg, char chr)
{
	uint16_t row_width = OSD->ysize;

#ifdef RENDER_SPEEDUP
	uint16_t *remap = fg == 2 ? remap_norm : remap_highlight;
#endif

	uint16_t *fb = OSDFB16;
	for (int i = 0; i < 8; i++)
#ifndef RENDER_SPEEDUP
	{
		uint8_t c8 = IBM_VGA_8x8[chr * 8 + i];
		uint16_t c16 =
			((((c8 >> 0) & 1) ? fg : bg) << (7 * 2)) |
			((((c8 >> 1) & 1) ? fg : bg) << (6 * 2)) |
			((((c8 >> 2) & 1) ? fg : bg) << (5 * 2)) |
			((((c8 >> 3) & 1) ? fg : bg) << (4 * 2)) |
			((((c8 >> 4) & 1) ? fg : bg) << (3 * 2)) |
			((((c8 >> 5) & 1) ? fg : bg) << (2 * 2)) |
			((((c8 >> 6) & 1) ? fg : bg) << (1 * 2)) |
			((((c8 >> 7) & 1) ? fg : bg) << (0 * 2));
		fb[(x + (y + i) * row_width) / 8] = c16;
	}
#else
		fb[(x + (y + i) * row_width) / 8] = remap[IBM_VGA_8x8[chr * 8 + i]];
#endif
}

void osd_draw_text(uint8_t row, uint8_t column, uint8_t bg, uint8_t fg, const char *text)
{
	if (!osd_enable)
	{
		OSD->enable = 0;
		return;
	}

	uint16_t row_width = OSD->ysize;

	for (int i = 0; i < 10; i++)
		memset(&OSDFB16[(column * OSD_CHAR_COLS * 8 + (row * 10 + i) * row_width) / 8], bg | (bg << 2) | (bg << 4) | (bg << 6), OSD_CHAR_COLS * 8 / 4);

	for (int i = 0; i < strlen(text); i++)
		osd_draw_char(column * OSD_CHAR_COLS * 8 + i * 8, row * 10, bg, fg, text[i]);
}