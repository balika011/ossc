#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "system.h"
#include "osd.h"
#include "menu.h"
#include "timer.h"

#include "osd_ibm_vga_font.h"

uint8_t osd_enable = 1, osd_status_timeout = 1;

char row1[LCD_ROW_LEN + 1], row2[LCD_ROW_LEN + 1];
char menu_row1[LCD_ROW_LEN + 1], menu_row2[LCD_ROW_LEN + 1];

char osd_menu_active = 0;

static int timer_idx = -1;

#define OSC_WIDTH 128
#define OSC_HEIGHT 384

int osd_notification_y = 0;

void osd_init()
{
	OSD->colors[0] = 0x00000000;
	OSD->colors[1] = 0x7f000000;
	OSD->colors[2] = 0xffffffff;
	OSD->colors[3] = 0xffffff00;
}

void osd_notification(uint8_t osd_mode)
{
	uint8_t menu_page;

	if ((osd_mode == 1) || (osd_enable == 2))
	{
		OSD->ysize = OSC_WIDTH;
		OSD->ypos = osd_notification_y;
		osd_clear();
		osd_draw_text(0, 0, 1, 2, menu_row1);
		if (menu_row2[0])
			osd_draw_text(1, 0, 1, 2, menu_row2);
	}
	else if (osd_mode == 2)
	{
		uint8_t menu_page = get_current_menunavi()->mp;
		osd_draw_text(menu_page, 1, 1, 3, menu_row2);
	}

	lcd_write(menu_row1, menu_row2);
}

void osd_status(uint8_t refresh_osd_timer)
{
	if (!menu_active)
	{
		if (refresh_osd_timer)
			osd_status_refresh();

		strncpy(menu_row1, row1, OSD_CHAR_COLS);
		strncpy(menu_row2, row2, OSD_CHAR_COLS);
		osd_notification(1);
	}
}

void osd_set_menu_active(char active)
{
	osd_menu_active = active;

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

static void osd_off()
{
	OSD->enable = 0;
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
		OSD->enable = 0;
		return;
	}

	OSD->enable = 1;

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
		if (osd_menu_active && OSD->enable == 0)
		{
			OSD->enable = 1;
			menu_render_page();
		}
	}
	else
		OSD->enable = 0;
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
	memset(OSDFB8, 0x00, 0x6000);
}

void osd_draw_char(uint16_t x, uint16_t y, uint8_t bg, uint8_t fg, char chr)
{
	uint16_t row_width = OSD->ysize;

	uint16_t *fb = OSDFB16;
	for (int i = 0; i < 8; i++)
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
}

void osd_draw_text(uint8_t row, uint8_t column, uint8_t bg, uint8_t fg, const char *text)
{
	uint16_t row_width = OSD->ysize;

	for (int i = 0; i < 10; i++)
		memset(&OSDFB16[(column * OSD_CHAR_COLS * 8 + (row * 10 + i) * row_width) / 8], bg | (bg << 2) | (bg << 4) | (bg << 6), OSD_CHAR_COLS * 8 / 4);

	for (int i = 0; i < strlen(text); i++)
		osd_draw_char(column * OSD_CHAR_COLS * 8 + i * 8, row * 10, bg, fg, text[i]);
}