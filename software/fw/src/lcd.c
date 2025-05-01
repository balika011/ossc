//
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

#include "lcd.h"
#include "st7032.h"
#include "sh1107.h"
#include "av_controller.h"
#include "timer.h"

static int has_sh1107;
static int timer_idx = -1;
uint8_t lcd_bl_timeout;

void lcd_init()
{
	has_sh1107 = sh1107_init();

	if (!has_sh1107)
		st7032_init();
}

static void lcd_off()
{
	if (has_sh1107)
		sh1107_off();
	else
		st7032_off();
}

static void lcd_reset_timeout()
{
	if (has_sh1107)
		sh1107_on();
	else
		st7032_on();

	if (timer_idx >= 0)
	{
		timer_cancel(timer_idx);
		timer_idx = -1;
	}

	if (lcd_bl_timeout)
	{
		int timeout = 0;
		switch (lcd_bl_timeout)
		{
			case 1: timeout = 3000000; break;
			case 2: timeout = 10000000; break;
			case 3: timeout = 30000000; break;	
		}

		timer_idx = timer_timeout(timeout, lcd_off);
	}
}

void lcd_write_row1(const char *str)
{
	if (has_sh1107)
		sh1107_write_row1(str);
	else
		st7032_write_row1(str);

	lcd_reset_timeout();
}

void lcd_write_row2(const char *str)
{
	if (has_sh1107)
		sh1107_write_row2(str);
	else
		st7032_write_row2(str);

	lcd_reset_timeout();
}

int lcd_has_sh1107()
{
	return has_sh1107;
}