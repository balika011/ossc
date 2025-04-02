//
// Copyright (C) 2015-2023  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#ifndef AV_CONTROLLER_H_
#define AV_CONTROLLER_H_

#include "avconfig.h"
#include "sysconfig.h"
#include "lcd.h"

#define AUTO_OFF                    0
#define AUTO_CURRENT_INPUT          1
#define AUTO_MAX_COUNT              100
#define AUTO_CURRENT_MAX_COUNT      6

#define PLL_CONFIG_VG               0

// In reverse order of importance
typedef enum {
    NO_CHANGE           = 0,
    SC_CONFIG_CHANGE    = 1,
    MODE_CHANGE         = 2,
    TX_MODE_CHANGE      = 3,
    ACTIVITY_CHANGE     = 4
} status_t;

typedef enum {
    TX_HDMI_RGB         = 0,
    TX_HDMI_YCBCR444    = 1,
    TX_DVI              = 2
} tx_mode_t;

typedef struct {
    uint32_t data[5];
} pll_config_t;

//TODO: transform binary values into flags
typedef struct {
    uint32_t totlines;
    uint32_t pcnt_field;
    uint32_t hsync_width;
    uint32_t clkcnt;
    uint8_t progressive;
    int8_t id;
    uint8_t sync_active;
    uint8_t hdmitx_pclk_level;
    uint32_t pclk_o_hz;
    avinput_t avinput;
    // Current configuration
    avconfig_t cc;
} avmode_t;

extern avmode_t cm;
extern avinput_t target_input;
extern uint8_t update_cur_vm;
extern uint8_t input_profiles[AV_LAST];
extern uint8_t profile_sel;
extern uint8_t def_input, profile_link;
extern uint8_t lcd_bl_timeout;
extern uint8_t auto_input, auto_av1_ypbpr, auto_av2_ypbpr, auto_av3_ypbpr;
extern uint8_t osd_enable, osd_status_timeout, phase_hotkey_enable;
extern char menu_row1[LCD_ROW_LEN + 1], menu_row2[LCD_ROW_LEN + 1];

void ui_disp_menu(uint8_t osd_mode);
void ui_disp_status(uint8_t refresh_osd_timer);

void set_sampler_phase(uint8_t sampler_phase, uint8_t update_sc);

int load_profile();
int save_profile();

void print_vm_stats();

#endif
