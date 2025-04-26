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

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "lcd.h"
#include "sh1107.h"
#include "i2c_opencores.h"
#include "font12.h"

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

static const uint8_t init[] = {
	0xae,		/*turn off OLED display*/
	0x00,		/*set lower column address*/
	0x10,		/*set higher column address*/
	0xb0,		/*set page address*/
	0xdc, 0x00, /*set display start line*/
	0x81,		/*contract control*/
	0x6f,		/*128*/
	0x21,		/* Set Memory addressing mode (0x20/0x21) */
	0xa1,		/*set segment remap*/
	0xc0,		/*Com scan direction*/
	0xa4,		/*Disable Entire Display On (0xA4/0xA5)*/
	0xa6,		/*normal / reverse*/
	0xa8,		/*multiplex ratio*/
	0x3f,		/*duty = 1/64*/
	0xd3, 0x60, /*set display offset*/
	0xd5, 0x41, /*set osc division*/
	0xd9, 0x22, /*set pre-charge period*/
	0xdb, 0x35, /*set vcomh*/
	0xad,		/*set charge pump enable*/
	0x8a		/*Set DC-DC enable (a=0:disable; a=1:enable) */
};

int sh1107_init()
{
	SC->sys_ctrl.lcd_cs_n = 0;
	SC->sys_ctrl.lcd_rs = 0;

	if (I2C_start(I2CA_BASE, 0x3c, 1) == I2C_NOACK)
		return 0;

	if ((I2C_read(I2CA_BASE, 1) & 0x3f) != 7)
		return 0;

	I2C_start(I2CA_BASE, 0x3c, 0);
	I2C_write(I2CA_BASE, 0x00, 0);
	for (int i = 0; i < sizeof(init); i++)
		I2C_write(I2CA_BASE, init[i], i == sizeof(init) - 1);

	// Clear the screen
	I2C_start(I2CA_BASE, 0x3c, 0);
	I2C_write(I2CA_BASE, 0x00, 0);
	I2C_write(I2CA_BASE, 0xb0, 1);
	for (uint8_t i = 0; i < OLED_HEIGHT; i++)
	{
		I2C_start(I2CA_BASE, 0x3c, 0);
		I2C_write(I2CA_BASE, 0x00, 0);
		I2C_write(I2CA_BASE, 0x00 + (i & 0x0f), 0);
		I2C_write(I2CA_BASE, 0x10 + (i >> 4), 1);

		I2C_start(I2CA_BASE, 0x3c, 0);
		I2C_write(I2CA_BASE, 0x40, 0);
		for (uint8_t i = 0; i < OLED_WIDTH / 8; i++)
			I2C_write(I2CA_BASE, 0x00, i == OLED_WIDTH / 8 - 1);
	}

	return 1;
}

void __attribute__((noinline, __section__(".rtext"))) sh1107_write(char *row1, char *row2)
{
	// Turn on
	I2C_start(I2CA_BASE, 0x3c, 0);
	I2C_write(I2CA_BASE, 0x00, 0);
	I2C_write(I2CA_BASE, 0xaf, 1);

	uint8_t row1len = LCD_ROW_LEN;
	for (int i = 0; i < LCD_ROW_LEN; i++)
		if (!row1[i])
		{
			row1len = i;
			break;
		}

	for (uint8_t i = 0; i < 12; i++)
	{
		I2C_start(I2CA_BASE, 0x3c, 0);
		I2C_write(I2CA_BASE, 0x00, 0);
		I2C_write(I2CA_BASE, 0xb0, 0);
		I2C_write(I2CA_BASE, 0x00 + ((i + 13) & 0x0f), 0);
		I2C_write(I2CA_BASE, 0x10 + ((i + 13) >> 4), 1);

		I2C_start(I2CA_BASE, 0x3c, 0);
		I2C_write(I2CA_BASE, 0x40, 0);
		for (uint8_t j = 0; j < row1len; j++)
			I2C_write(I2CA_BASE, Font12_Table[(row1[j] - 0x20) * 12 + i], j == LCD_ROW_LEN - 1);

		for (uint8_t j = row1len; j < LCD_ROW_LEN; j++)
			I2C_write(I2CA_BASE, Font12_Table[i], j == LCD_ROW_LEN - 1);
	}

	uint8_t row2len = LCD_ROW_LEN;
	for (int i = 0; i < LCD_ROW_LEN; i++)
		if (!row2[i])
		{
			row2len = i;
			break;
		}

	for (uint8_t i = 0; i < 12; i++)
	{
		I2C_start(I2CA_BASE, 0x3c, 0);
		I2C_write(I2CA_BASE, 0x00, 0);
		I2C_write(I2CA_BASE, 0xb0, 0);
		I2C_write(I2CA_BASE, 0x00 + ((i + 39) & 0x0f), 0);
		I2C_write(I2CA_BASE, 0x10 + ((i + 39) >> 4), 1);

		I2C_start(I2CA_BASE, 0x3c, 0);
		I2C_write(I2CA_BASE, 0x40, 0);
		for (uint8_t j = 0; j < row2len; j++)
			I2C_write(I2CA_BASE, Font12_Table[(row2[j] - 0x20) * 12 + i], j == LCD_ROW_LEN - 1);

		for (uint8_t j = row2len; j < LCD_ROW_LEN; j++)
			I2C_write(I2CA_BASE, Font12_Table[i], j == LCD_ROW_LEN - 1);
	}
}

int sh1107_off()
{
	// Turn off
	I2C_start(I2CA_BASE, 0x3c, 0);
	I2C_write(I2CA_BASE, 0x00, 0);
	I2C_write(I2CA_BASE, 0xae, 1);
}