#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "system.h"
#include "osd.h"
#include "menu.h"
#include "timer.h"

uint8_t osd_enable = 1, osd_status_timeout = 1;

char row1[LCD_ROW_LEN + 1], row2[LCD_ROW_LEN + 1];
char menu_row1[LCD_ROW_LEN + 1], menu_row2[LCD_ROW_LEN + 1];

char osd_menu_active = 0;

static int timer_idx = -1;

void osd_init()
{
	OSD->osd_config.enable = 0;
	OSD->osd_config.x_size = 0;
	OSD->osd_config.y_size = 0;
	OSD->osd_config.x_offset = 3;
	OSD->osd_config.y_offset = 3;
	OSD->osd_config.border_color = 1;
}

void ui_disp_menu(uint8_t osd_mode)
{
	uint8_t menu_page;

	if ((osd_mode == 1) || (osd_enable == 2))
	{
		strncpy((char *)OSD->osd_array.data[0][0], menu_row1, OSD_CHAR_COLS);
		strncpy((char *)OSD->osd_array.data[1][0], menu_row2, OSD_CHAR_COLS);
		OSD->osd_row_color.mask = 0;
		OSD->osd_sec_enable[0].mask = menu_row2[0] ? 3 : 1;
		OSD->osd_sec_enable[1].mask = 0;
	}
	else if (osd_mode == 2)
	{
		menu_page = get_current_menunavi()->mp;
		strncpy((char *)OSD->osd_array.data[menu_page][1], menu_row2, OSD_CHAR_COLS);
		OSD->osd_sec_enable[1].mask |= (1 << menu_page);
	}

	lcd_write(menu_row1, menu_row2);
}

void ui_disp_status(uint8_t refresh_osd_timer)
{
	if (!menu_active)
	{
		if (refresh_osd_timer)
			osd_status_refresh();

		strncpy((char *)OSD->osd_array.data[0][0], row1, OSD_CHAR_COLS);
		strncpy((char *)OSD->osd_array.data[1][0], row2, OSD_CHAR_COLS);
		OSD->osd_row_color.mask = 0;
		OSD->osd_sec_enable[0].mask = 3;
		OSD->osd_sec_enable[1].mask = 0;

		lcd_write(row1, row2);
	}
}

void osd_set_menu_active(char active)
{
	osd_menu_active = active;

	if (!osd_enable)
	{
		OSD->osd_config.enable = 0;
		return;
	}

	if (osd_menu_active)
	{
		if (timer_idx >= 0)
		{
			timer_cancel(timer_idx);
			timer_idx = -1;
		}

		OSD->osd_config.enable = 1;
	}
	else
	{
		if (timer_idx == -1)
			OSD->osd_config.enable = 0;
	}
}

static void osd_off()
{
	OSD->osd_config.enable = 0;
}

void osd_status_refresh()
{
	if (timer_idx >= 0)
	{
		timer_cancel(timer_idx);
		timer_idx = -1;
	}

	if (!osd_enable)
	{
		OSD->osd_config.enable = 0;
		return;
	}

	OSD->osd_config.enable = 1;

	if (osd_status_timeout)
	{
		int timeout = 0;
		switch (osd_status_timeout)
		{
			case 0: timeout = 2000000; break;
			case 1: timeout = 5000000; break;
			case 2: timeout = 10000000; break;
		}

		timer_idx = timer_timeout(timeout, osd_off);
	}
}

void osd_update()
{
	if (osd_enable)
	{
		if (osd_menu_active)
		{
			OSD->osd_config.enable = 1;
			render_osd_page();
		}
	}
	else
		OSD->osd_config.enable = 0;
}

void osd_update_size(mode_data_t *vm_out)
{
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
}