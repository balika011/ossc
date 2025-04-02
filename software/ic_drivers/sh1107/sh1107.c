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

extern uint32_t sys_ctrl;

static void OLED_Write(uint8_t value)
{
	SPI_write(I2CA_BASE, &value, 1);
}

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

void sh1107_init()
{
	SC->sys_ctrl.lcd_cs_n = 0;
	SC->sys_ctrl.lcd_rs = 0;
    SPI_write(I2CA_BASE, init, sizeof(init));

    // Clear the screen
    OLED_Write(0xb0);
    for (uint8_t i = 0; i < OLED_HEIGHT; i++)
	{
		SC->sys_ctrl.lcd_rs = 0;
        OLED_Write(0x00 + (i & 0x0f));
        OLED_Write(0x10 + (i >> 4));

		SC->sys_ctrl.lcd_rs = 1;
        for (uint8_t i = 0; i < OLED_WIDTH / 8; i++)
            OLED_Write(0);
	}

	SC->sys_ctrl.lcd_rs = 0;

	// Turn on
    OLED_Write(0xaf);

	SC->sys_ctrl.lcd_cs_n = 1;
}

void sh1107_write(char *row1, char *row2)
{
	SC->sys_ctrl.lcd_cs_n = 0;
	SC->sys_ctrl.lcd_rs = 0;

    uint8_t row1len = strnlen(row1, LCD_ROW_LEN);

    for (uint8_t i = 0; i < 12; i++)
	{
		SC->sys_ctrl.lcd_rs = 0;
		OLED_Write(0xb0);
        OLED_Write(0x00 + ((i + 13) & 0x0f));
        OLED_Write(0x10 + ((i + 13) >> 4));

		SC->sys_ctrl.lcd_rs = 1;
        for (uint8_t j = 0; j < row1len; j++)
            OLED_Write(Font12_Table[(row1[j] - 0x20) * 12 + i]);

        for (uint8_t j = row1len; j < LCD_ROW_LEN; j++)
            OLED_Write(Font12_Table[i]);
	}

	uint8_t row2len = strnlen(row2, LCD_ROW_LEN);

    for (uint8_t i = 0; i < 12; i++)
	{
		SC->sys_ctrl.lcd_rs = 0;
        OLED_Write(0xb0);
        OLED_Write(0x00 + ((i + 39) & 0x0f));
        OLED_Write(0x10 + ((i + 39) >> 4));

		SC->sys_ctrl.lcd_rs = 1;
		for (uint8_t j = 0; j < row2len; j++)
            OLED_Write(Font12_Table[(row2[j] - 0x20) * 12 + i]);

        for (uint8_t j = row2len; j < LCD_ROW_LEN; j++)
            OLED_Write(Font12_Table[i]);
	}

	SC->sys_ctrl.lcd_cs_n = 1;
}
