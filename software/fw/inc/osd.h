#ifndef OSD_H_
#define OSD_H_

#include <stdint.h>
#include "lcd.h"
#include "video_modes.h"

extern char row1[LCD_ROW_LEN + 1], row2[LCD_ROW_LEN + 1];
extern char menu_row1[LCD_ROW_LEN + 1], menu_row2[LCD_ROW_LEN + 1];
extern uint8_t osd_enable, osd_status_timeout;

void osd_init();
void ui_disp_menu(uint8_t osd_mode);
void ui_disp_status(uint8_t refresh_osd_timer);
void osd_set_menu_active(char active);
void osd_status_refresh();
void osd_update();
void osd_update_size(mode_data_t *vm_out);

#endif