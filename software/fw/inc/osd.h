#ifndef OSD_H_
#define OSD_H_

#include <stdint.h>
#include "lcd.h"
#include "video_modes.h"

extern uint8_t osd_enable, osd_status_timeout;

void osd_init();
void osd_notification(const char *row1, const char *row2);
void osd_status(const char *row1, const char *row2);
void osd_set_menu_active(char active);
void osd_update_size(mode_data_t *vm_out);

void osd_clear();
void osd_draw_char(uint16_t x, uint16_t y, uint8_t bg, uint8_t fg, char chr);
void osd_draw_text(uint8_t row, uint8_t column, uint8_t bg, uint8_t fg, const char *text);

#endif