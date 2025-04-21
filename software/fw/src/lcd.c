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

static int has_sh1107;

void lcd_init()
{
	has_sh1107 = sh1107_init();

	if (!has_sh1107)
		st7032_init();
}

void lcd_write(char *row1, char *row2)
{
	if (has_sh1107)
		sh1107_write(row1, row2);
	else
		st7032_write(row1, row2);
}
int lcd_has_sh1107()
{
	return has_sh1107;
}