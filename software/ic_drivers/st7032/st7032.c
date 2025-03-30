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

#include <unistd.h>
#include <string.h>
#include "lcd.h"
#include "st7032.h"
#include "alt_types.h"
#include "altera_avalon_pio_regs.h"
#include "i2c_opencores.h"

#define WRDELAY     20
#define CLEARDELAY  800

extern alt_u32 sys_ctrl;

static void lcd_cmd(alt_u8 cmd, alt_u16 postdelay) {
    SPI_write(I2CA_BASE, &cmd, 1);
    usleep(postdelay);
}

void st7032_init()
{
	SC->sys_ctrl.lcd_cs_n = 0;
	SC->sys_ctrl.lcd_rs = 0;
	usleep(WRDELAY);

    lcd_cmd(0x38, WRDELAY);    // function set
    lcd_cmd(0x39, WRDELAY);    // function set, select extended table (IS=1)
    lcd_cmd(0x14, WRDELAY);    // osc freq
    lcd_cmd(0x71, WRDELAY);    // contrast set
    lcd_cmd(0x5E, WRDELAY);    // power/icon/cont
    lcd_cmd(0x6D, WRDELAY);    // follower control
    lcd_cmd(0x0C, WRDELAY);    // display on
    lcd_cmd(0x01, CLEARDELAY); // clear display
    lcd_cmd(0x06, WRDELAY);    // entry mode set
    lcd_cmd(0x02, CLEARDELAY); // return home

	SC->sys_ctrl.lcd_cs_n = 1;
}

void st7032_write(char *row1, char *row2)
{
    alt_u8 i, rowlen;

	SC->sys_ctrl.lcd_cs_n = 0;
	SC->sys_ctrl.lcd_rs = 0;

	lcd_cmd(0x01,CLEARDELAY); // clear display
    usleep(400); // additional delay for copycat lcd module

	// Set RS to enter data write mode
	SC->sys_ctrl.lcd_rs = 1;

    //ensure no empty row
    rowlen = strnlen(row1, LCD_ROW_LEN);
    if (rowlen == 0) {
        strncpy(row1, " ", LCD_ROW_LEN+1);
        rowlen++;
    }

    for (i=0; i<rowlen; i++)
        lcd_cmd(row1[i],WRDELAY);

	// second row
	SC->sys_ctrl.lcd_rs = 0;
    lcd_cmd((1<<7)|0x40,WRDELAY);

	SC->sys_ctrl.lcd_rs = 1;

	//ensure no empty row
    rowlen = strnlen(row2, LCD_ROW_LEN);
    if (rowlen == 0) {
        strncpy(row2, " ", LCD_ROW_LEN+1);
        rowlen++;
    }

    for (i=0; i<rowlen; i++)
        lcd_cmd(row2[i],WRDELAY);

	SC->sys_ctrl.lcd_cs_n = 1;
}
