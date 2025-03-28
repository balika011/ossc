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

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "system.h"
#include "alt_types.h"
#include "controls.h"
#include "menu.h"
#include "av_controller.h"
#include "video_modes.h"
#include "userdata.h"
#include "firmware.h"
#include "lcd.h"
#include "altera_avalon_pio_regs.h"

static const char *rc_keydesc[REMOTE_MAX_KEYS] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", \
                                                   "MENU", "OK", "BACK", "UP", "DOWN", "LEFT", "RIGHT", "INFO", "LCD_BACKLIGHT", "SCANLINE_MODE", \
                                                   "SCANLINE_TYPE", "SCANLINE_INT+", "SCANLINE_INT-", "LINEMULT_MODE", "PHASE+", "PHASE-", "PROFILE_HOTKEY"};
const alt_u16 rc_keymap_default[REMOTE_MAX_KEYS] = {0x3E29, 0x3EA9, 0x3E69, 0x3EE9, 0x3E19, 0x3E99, 0x3E59, 0x3ED9, 0x3E39, 0x3EC9, \
                                              0x3E4D, 0x3E1D, 0x3EED, 0x3E2D, 0x3ECD, 0x3EAD, 0x3E6D, 0x3E65, 0x3E01, 0x1C48, \
                                              0x1C18, 0x1C50, 0x1CD0, 0x1CC8, 0x5E58, 0x5ED8, 0x3EB9};
alt_u16 rc_keymap[REMOTE_MAX_KEYS];

extern char menu_row1[LCD_ROW_LEN+1], menu_row2[LCD_ROW_LEN+1];
extern mode_data_t video_modes_plm[];
extern avmode_t cm;
extern avconfig_t tc;
extern avinput_t target_input;
extern alt_u8 menu_active;
extern alt_u32 sys_ctrl;
extern alt_u16 tc_sampler_phase;
extern alt_u8 profile_sel, profile_sel_menu;
extern alt_u8 lcd_bl_timeout;
extern alt_u8 vm_edit;
extern volatile osd_regs *osd;

#if 0
extern menu_t menu_scanlines, menu_advtiming;
#endif

alt_u32 remote_code;
alt_u8 remote_rpt, remote_rpt_prev;
alt_u32 btn_code, btn_code_prev;
alt_u8 phase_hotkey_enable;

void setup_rc()
{
    int i, confirm;
    alt_u32 remote_code_prev = 0;

    for (i=0; i<REMOTE_MAX_KEYS; i++) {
        strncpy(menu_row1, "Press", LCD_ROW_LEN+1);
        strncpy(menu_row2, rc_keydesc[i], LCD_ROW_LEN+1);
        osd->osd_config.menu_active = 1;
        ui_disp_menu(1);
        confirm = 0;

        while (1) {
            remote_code = IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE) & RC_MASK;
            btn_code = ~IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE) & PB_MASK;

            if (remote_code && (remote_code != remote_code_prev)) {
                if (confirm == 0) {
                    rc_keymap[i] = remote_code;
                    strncpy(menu_row1, "Confirm", LCD_ROW_LEN+1);
                    ui_disp_menu(1);
                    confirm = 1;
                } else {
                    if (remote_code == rc_keymap[i]) {
                        confirm = 2;
                    } else {
                        strncpy(menu_row1, "Mismatch, retry", LCD_ROW_LEN+1);
                        ui_disp_menu(1);
                        confirm = 0;
                    }
                }
            }

            if ((btn_code_prev == 0) && (btn_code == PB0_BIT)) {
                if (i == 0) {
                    memcpy(rc_keymap, rc_keymap_default, sizeof(rc_keymap));
                    i=REMOTE_MAX_KEYS;
                } else {
                    i-=2;
                }
                confirm = 2;
            }

            remote_code_prev = remote_code;
            btn_code_prev = btn_code;

            if (confirm == 2)
                break;

            usleep(WAITLOOP_SLEEP_US);
        }
    }
    write_userdata(INIT_CONFIG_SLOT);

    osd->osd_config.menu_active = 0;
}

int parse_control()
{
    int i, prof_x10=0, ret=0, retval;
    alt_u32 btn_vec, btn_vec_prev=1;
    alt_u8 pt_only = 0;
    avinput_t man_target_input = AV_LAST;

    // one for each video_group
    alt_u8* pmcfg_ptr[] = { &pt_only, &tc.pm_240p, &tc.pm_240p, &tc.pm_384p, &tc.pm_480i, &tc.pm_480i, &tc.pm_480p, &tc.pm_480p, &pt_only, &tc.pm_1080i, &pt_only };
    alt_u8 valid_pm[] = { 0x1, 0x3f, 0x3f, 0x7, 0xf, 0xf, 0x7, 0x7, 0x1, 0x3, 0x1 };

    avinput_t next_input = (cm.avinput == AV3_YPBPR) ? AV1_RGBs : (cm.avinput+1);

    if (remote_code)
        printf("RCODE: 0x%.4lx, %d\n", remote_code, remote_rpt);

    if (btn_code)
        printf("BCODE: 0x%.2lx\n", btn_code>>16);

    for (i = RC_BTN1; i < REMOTE_MAX_KEYS; i++) {
        if (remote_code == rc_keymap[i])
            break;
        if (i == REMOTE_MAX_KEYS - 1)
            goto Button_Check;
    }

    switch (i) {
        case RC_BTN1: man_target_input = AV1_RGBs; break;
        case RC_BTN4: man_target_input = AV1_RGsB; break;
        case RC_BTN7: man_target_input = AV1_YPBPR; break;
        case RC_BTN2: man_target_input = AV2_YPBPR; break;
        case RC_BTN5: man_target_input = AV2_RGsB; break;
        case RC_BTN3: man_target_input = AV3_RGBHV; break;
        case RC_BTN6: man_target_input = AV3_RGBs; break;
        case RC_BTN9: man_target_input = AV3_RGsB; break;
        case RC_BTN0: man_target_input = AV3_YPBPR; break;
        case RC_MENU:
            menu_active = !menu_active;
            osd->osd_config.menu_active = menu_active;
            profile_sel_menu = profile_sel;

            if (menu_active) {
                render_osd_page();
                display_menu(1);
            } else {
                ui_disp_status(0);
            }

            break;
        case RC_INFO:
            print_vm_stats();
            break;
        case RC_LCDBL:
            sys_ctrl ^= LCD_BL;
            break;
        case RC_SL_MODE:
            tc.sl_mode = (tc.sl_mode < SL_MODE_MAX) ? (tc.sl_mode + 1) : 0;
#if 0
            if (!menu_active) {
                strncpy((char*)osd->osd_array.data[0][0], menu_scanlines.items[0].name, OSD_CHAR_COLS);
                strncpy((char*)osd->osd_array.data[1][0], menu_scanlines.items[0].sel.setting_str[tc.sl_mode], OSD_CHAR_COLS);
                osd->osd_config.status_refresh = 1;
                osd->osd_row_color.mask = 0;
                osd->osd_sec_enable[0].mask = 3;
                osd->osd_sec_enable[1].mask = 0;
            } else if (get_current_menunavi()->m == &menu_scanlines) {
                render_osd_page();
            }
#endif
            break;
        case RC_SL_TYPE:
			tc.sl_type = (tc.sl_type < SL_TYPE_MAX) ? (tc.sl_type + 1) : 0;
#if 0
            if (!menu_active) {
                strncpy((char*)osd->osd_array.data[0][0], menu_scanlines.items[6].name, OSD_CHAR_COLS);
                strncpy((char*)osd->osd_array.data[1][0], menu_scanlines.items[6].sel.setting_str[tc.sl_type], OSD_CHAR_COLS);
                osd->osd_config.status_refresh = 1;
                osd->osd_row_color.mask = 0;
                osd->osd_sec_enable[0].mask = 3;
                osd->osd_sec_enable[1].mask = 0;
            } else if (get_current_menunavi()->m == &menu_scanlines) {
                render_osd_page();
            }
#endif
			break;
        case RC_SL_MINUS:
        case RC_SL_PLUS:
            if (i == RC_SL_MINUS)
                tc.sl_str = tc.sl_str ? (tc.sl_str - 1) : 0;
            else
                tc.sl_str = (tc.sl_str < SCANLINESTR_MAX) ? (tc.sl_str + 1) : SCANLINESTR_MAX;

#if 0
            if (!menu_active) {
                strncpy((char*)osd->osd_array.data[0][0], menu_scanlines.items[1].name, OSD_CHAR_COLS);
                menu_scanlines.items[1].num.df(tc.sl_str);
                strncpy((char*)osd->osd_array.data[1][0], menu_row2, OSD_CHAR_COLS);
                osd->osd_config.status_refresh = 1;
                osd->osd_row_color.mask = 0;
                osd->osd_sec_enable[0].mask = 3;
                osd->osd_sec_enable[1].mask = 0;
            } else if (get_current_menunavi()->m == &menu_scanlines) {
                render_osd_page();
			}
#endif
			break;
        case RC_LM_MODE:
            strncpy(menu_row1, "Linemult mode:", LCD_ROW_LEN+1);
            strncpy(menu_row2, "press 1-6", LCD_ROW_LEN+1);
            osd->osd_config.menu_active = 1;
            ui_disp_menu(1);

            while (1) {
                btn_vec = IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE) & RC_MASK;
                for (i = RC_BTN1; i < REMOTE_MAX_KEYS; i++) {
                    if (btn_vec == rc_keymap[i])
                        break;
                }

                if (video_modes_plm[cm.id].group > GROUP_1080P) {
                    printf("WARNING: Corrupted mode (id %d)\n", cm.id);
                    break;
                }

                if (i <= RC_BTN6) {
                    if ((1<<i) & valid_pm[video_modes_plm[cm.id].group]) {
                        *pmcfg_ptr[video_modes_plm[cm.id].group] = i;
                    } else {
                        sniprintf(menu_row2, LCD_ROW_LEN+1, "%ux unsupported", i+1);
                        ui_disp_menu(1);
                        usleep(500000);
                    }
                    break;
                } else if (i == RC_BACK) {
                    break;
                }

                usleep(WAITLOOP_SLEEP_US);
            }
            menu_active = 0;
            osd->osd_config.menu_active = 0;
            ui_disp_status(0);
            break;
        case RC_PHASE_MINUS:
        case RC_PHASE_PLUS:
            if (phase_hotkey_enable) {
                if (i == RC_PHASE_MINUS)
                    video_modes_plm[cm.id].sampler_phase = video_modes_plm[cm.id].sampler_phase ? (video_modes_plm[cm.id].sampler_phase - 1) : SAMPLER_PHASE_MAX;
                else
                    video_modes_plm[cm.id].sampler_phase = (video_modes_plm[cm.id].sampler_phase < SAMPLER_PHASE_MAX) ? (video_modes_plm[cm.id].sampler_phase + 1) : 0;

                if (cm.id == vm_edit)
                    tc_sampler_phase = video_modes_plm[cm.id].sampler_phase;

                set_sampler_phase(video_modes_plm[cm.id].sampler_phase, 1);

#if 0
                if (!menu_active) {
                    strncpy((char*)osd->osd_array.data[0][0], menu_advtiming.items[10].name, OSD_CHAR_COLS);
                    sampler_phase_disp(video_modes_plm[cm.id].sampler_phase);
                    strncpy((char*)osd->osd_array.data[1][0], menu_row2, OSD_CHAR_COLS);
                    osd->osd_config.status_refresh = 1;
                    osd->osd_row_color.mask = 0;
                    osd->osd_sec_enable[0].mask = 3;
                    osd->osd_sec_enable[1].mask = 0;
                } else if (get_current_menunavi()->m == &menu_advtiming) {
                    render_osd_page();
                }
#endif
            }
            break;
        case RC_PROF_HOTKEY:
Prof_Hotkey_Prompt:
            strncpy(menu_row1, "Profile load:", LCD_ROW_LEN+1);
            sniprintf(menu_row2, LCD_ROW_LEN+1, "press %u-%u", prof_x10*10, ((prof_x10*10+9) > MAX_PROFILE) ? MAX_PROFILE : (prof_x10*10+9));
            osd->osd_config.menu_active = 1;
            ui_disp_menu(1);

            while (1) {
                btn_vec = IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE) & RC_MASK;

                if ((btn_vec_prev == 0) && (btn_vec != 0)) {
                    for (i = RC_BTN1; i < REMOTE_MAX_KEYS; i++) {
                        if (btn_vec == rc_keymap[i])
                            break;
                    }

                    if ((i == RC_BTN0) || (i < (RC_BTN1 + (prof_x10 == (MAX_PROFILE/10)) ? (MAX_PROFILE%10) : 9))) {
                        profile_sel_menu = prof_x10*10 + ((i+1)%10);
                        retval = load_profile();
                        sniprintf(menu_row2, LCD_ROW_LEN+1, "%s", (retval==0) ? "Done" : "Failed");
                        ui_disp_menu(1);
                        usleep(500000);
                        break;
                    } else if (i == RC_PROF_HOTKEY) {
                        prof_x10 = (prof_x10+1) % ((MAX_PROFILE/10)+1);
                        btn_vec_prev = btn_vec;
                        goto Prof_Hotkey_Prompt;
                    } else if (i == RC_BACK) {
                        break;
                    }
                }

                btn_vec_prev = btn_vec;
                usleep(WAITLOOP_SLEEP_US);
            }

            menu_active = 0;
            osd->osd_config.menu_active = 0;
            ui_disp_status(0);
            break;
        case RC_RIGHT:
            if (!menu_active)
                man_target_input = next_input;
            break;
        default: break;
    }

    sys_ctrl ^= REMOTE_EVENT;

Button_Check:
    if (btn_code & PB0_BIT)
        man_target_input = next_input;
    if (btn_code & PB1_BIT)
        tc.sl_mode = tc.sl_mode < SL_MODE_MAX ? tc.sl_mode + 1 : 0;

    if (man_target_input != AV_LAST) {
        target_input = man_target_input;
        ret = 1;
    }

    sys_ctrl &= ~(3<<LCD_BL_TIMEOUT_OFFS);
    if (!menu_active)
        sys_ctrl |= (lcd_bl_timeout << LCD_BL_TIMEOUT_OFFS);

    IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, sys_ctrl);

    return ret;
}
