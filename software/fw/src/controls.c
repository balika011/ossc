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

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "system.h"
#include "controls.h"
#include "menu.h"
#include "av_controller.h"
#include "video_modes.h"
#include "userdata.h"
#include "lcd.h"
#include "timer.h"
#include "osd.h"

static const char *rc_keydesc[REMOTE_MAX_KEYS] = {
	"1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
	"MENU", "OK", "BACK", "UP", "DOWN", "LEFT", "RIGHT",
	"INFO", "STANDBY",
	"SCANLINE MODE", "SCANLINE TYPE", "SCANLINE INT+", "SCANLINE INT-",
	"LINEMULT MODE", "PHASE+", "PHASE-",
	"PROFILE LOAD"};

const uint16_t rc_keymap_old[REMOTE_MAX_KEYS] = {
	0x3e29, 0x3ea9, 0x3e69, 0x3ee9, 0x3e19, 0x3e99, 0x3e59, 0x3ed9, 0x3e39, 0x3ec9,
	0x3e4d, 0x3e1d,	0x3eed, 0x3e2d, 0x3ecd, 0x3ead, 0x3e6d,
	0x3e65, 0x3e01,
	0x1c48, 0x1c18, 0x1c50, 0x1cd0,
	0x1cc8, 0x5e58, 0x5ed8,
	0x3eb9
};

const uint16_t rc_keymap_L336[REMOTE_MAX_KEYS] = {
	0xe1, 0x61, 0xa1, 0xd1, 0x51, 0x91, 0xf1, 0x71, 0xb1, 0x49,
	0xe9, 0x79, 0x25, 0xa9, 0x59, 0xd9, 0x99,
	0x11, 0x15,
	0x6b, 0x31, 0x41, 0x19,
	0xc9, 0xc1, 0x09,
	0x21
};

const uint16_t rc_keymap_pro[REMOTE_MAX_KEYS] = {
	0x0ab0, 0x0a70, 0x0af0, 0x0a38, 0x0ab8, 0x0a78, 0x0af8, 0x0a20, 0x0aa0, 0x0a30,
	0x0aca, 0x0acc, 0x0a26, 0x0a0e, 0x0a8e, 0x0ace, 0x0a4e,
	0x0a08, 0x0ae8,
	0x0a50, 0x0ad0, 0x0a48, 0x0aa8,
	0x0a7a, 0x0a98, 0x0a18,
	0x0a88
};

uint16_t rc_keymap[REMOTE_MAX_KEYS];

uint32_t remote_code;
uint8_t remote_cnt, remote_cnt_prev;
uint32_t btn1, btn1_prev;
uint32_t btn2, btn2_prev;
uint8_t phase_hotkey_enable;

void controls_set_default()
{
	memcpy(rc_keymap, rc_keymap_old, sizeof(rc_keymap));
}

void controls_setup()
{
	osd_set_menu_active(0);

	strncpy(menu_row1, "Sel. Remote", sizeof(menu_row1));

	const char *remote_names[] = { "Old", "L336", "Pro", "Custom" };
	const uint16_t *remote_maps[] = { rc_keymap_old, rc_keymap_L336, rc_keymap_pro, 0 };
	int remote_idx = 0;

	while (1)
	{
		strncpy(menu_row2, remote_names[remote_idx], sizeof(menu_row2));
		ui_disp_menu(1);

		controls_update();

		if (!btn1_prev && btn1)
		{
			remote_idx = (remote_idx + 1) % (sizeof(remote_names) / sizeof(remote_names[0]));
		}
		else if (!btn2_prev && btn2)
		{
			if (remote_maps[remote_idx])
				memcpy(rc_keymap, remote_maps[remote_idx], sizeof(rc_keymap));
			else
			{
				for (int i = 0; i < REMOTE_MAX_KEYS; i++)
				{
					strncpy(menu_row1, "Press", sizeof(menu_row1));
					strncpy(menu_row2, rc_keydesc[i], sizeof(menu_row2));
					ui_disp_menu(1);
					uint32_t remote_code_prev = 0;

					while (1)
					{
						controls_update();

						if (remote_code)
						{
							if (!remote_code_prev)
							{
								rc_keymap[i] = remote_code;
								strncpy(menu_row1, "Confirm", sizeof(menu_row1));
								ui_disp_menu(1);
							}
							else if (remote_code != remote_code_prev)
							{
								strncpy(menu_row1, "Mismatch, retry", sizeof(menu_row1));
								ui_disp_menu(1);
								remote_code_prev = 0;
								continue;
							}
							else
							{
								remote_code_prev = 0;
								break;
							}
						}

						if ((!btn1_prev && btn1) || (!btn2_prev && btn2))
						{
							if (i == 0)
								i = REMOTE_MAX_KEYS;
							else
								i -= 2;

							break;
						}

						if (remote_code)
							remote_code_prev = remote_code;

						usleep(WAITLOOP_SLEEP_US);
					}
				}
			}
			break;
		}

		usleep(WAITLOOP_SLEEP_US);
	}

	strncpy(menu_row1, "Saving...", sizeof(menu_row1));
	menu_row2[0] = 0;
	ui_disp_menu(1);

	userdata_save_initconfig();

	osd_set_menu_active(0);
	ui_disp_status(0);
}

static void controls_reset_led()
{
	if (!in_standby)
		SC->sys_ctrl.led_g = 1;
}

void controls_update()
{
	remote_cnt_prev = remote_cnt;

	if (!btn1_prev && !btn2_prev)
	{
		btn1_prev = btn1;
		btn2_prev = btn2;
	}
	else
	{
		btn1_prev = btn1;
		btn2_prev = btn2;
		btn1 = 0;
		btn2 = 0;
	}

	remote_code = SC->controls.ir_code;
	btn1 = SC->controls.btn1;
	btn2 = SC->controls.btn2;
	remote_cnt = SC->controls.ir_cnt;

	if ((remote_cnt != 1 && remote_cnt < 6) || remote_cnt == remote_cnt_prev)
		remote_code = 0;

	if (remote_code)
	{
		SC->sys_ctrl.led_g = 0;
		timer_timeout(150000, controls_reset_led);
	}
}

int	controls_parse()
{
	int prof_x10 = 0, ret = 0;
	uint8_t pt_only = 0;
	avinput_t man_target_input = AV_LAST;

	// one for each video_group
	uint8_t *pmcfg_ptr[] = {&pt_only, &tc.pm_240p, &tc.pm_240p, &tc.pm_384p, &tc.pm_480i, &tc.pm_480i, &tc.pm_480p, &tc.pm_480p, &pt_only, &tc.pm_1080i, &pt_only};
	uint8_t valid_pm[] = {0x1, 0x3f, 0x3f, 0x7, 0xf, 0xf, 0x7, 0x7, 0x1, 0x3, 0x1};

	avinput_t next_input = (cm.avinput == AV3_YPBPR) ? AV1_RGBs : (cm.avinput + 1);

	if (remote_code)
		printf("RCODE: 0x%.4lx, %d\n", remote_code, remote_cnt);

	if (btn1 || btn2)
		printf("BCODE: %d %d\n", btn1, btn2);

	key_code_t code = KEY_NONE;

	for (int i = RC_BTN1; i < REMOTE_MAX_KEYS; i++)
	{
		if (remote_code == rc_keymap[i])
		{
			code = i;
			break;
		}
	}
	if (code == KEY_NONE)
	{
		if (btn1)
			code = FP_BTN1;
		else if (btn2)
			code = FP_BTN2;
	}

	if (in_standby)
	{
		switch (code)
		{
			case RC_STANDBY:
			case FP_BTN1:
			case FP_BTN2:
				exit_standby();
				break;
		}
	}
	else
	{
		switch (code)
		{
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
				osd_set_menu_active(menu_active);
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
			case RC_STANDBY:
				enter_standby();
				break;
			case RC_SL_MODE:
				tc.sl_mode = (tc.sl_mode < SL_MODE_MAX) ? (tc.sl_mode + 1) : 0;
				if (!menu_active) {
					strncpy((char*)OSD->osd_array.data[0][0], menu_scanlines.items[0].name, OSD_CHAR_COLS);
					strncpy((char*)OSD->osd_array.data[1][0], menu_scanlines.items[0].sel.setting_str[tc.sl_mode], OSD_CHAR_COLS);
					osd_status_refresh();
					OSD->osd_row_color.mask = 0;
					OSD->osd_sec_enable[0].mask = 3;
					OSD->osd_sec_enable[1].mask = 0;
				} else if (get_current_menunavi()->m == &menu_scanlines) {
					render_osd_page();
				}
				break;
			case RC_SL_TYPE:
				tc.sl_type = (tc.sl_type < SL_TYPE_MAX) ? (tc.sl_type + 1) : 0;
				if (!menu_active) {
					strncpy((char*)OSD->osd_array.data[0][0], menu_scanlines.items[6].name, OSD_CHAR_COLS);
					strncpy((char*)OSD->osd_array.data[1][0], menu_scanlines.items[6].sel.setting_str[tc.sl_type], OSD_CHAR_COLS);
					osd_status_refresh();
					OSD->osd_row_color.mask = 0;
					OSD->osd_sec_enable[0].mask = 3;
					OSD->osd_sec_enable[1].mask = 0;
				} else if (get_current_menunavi()->m == &menu_scanlines) {
					render_osd_page();
				}
				break;
			case RC_SL_MINUS:
			case RC_SL_PLUS:
				if (code == RC_SL_MINUS)
					tc.sl_str = tc.sl_str ? (tc.sl_str - 1) : 0;
				else
					tc.sl_str = (tc.sl_str < SCANLINESTR_MAX) ? (tc.sl_str + 1) : SCANLINESTR_MAX;

				if (!menu_active) {
					strncpy((char*)OSD->osd_array.data[0][0], menu_scanlines.items[1].name, OSD_CHAR_COLS);
					menu_scanlines.items[1].num.df(tc.sl_str);
					strncpy((char*)OSD->osd_array.data[1][0], menu_row2, OSD_CHAR_COLS);
					osd_status_refresh();
					OSD->osd_row_color.mask = 0;
					OSD->osd_sec_enable[0].mask = 3;
					OSD->osd_sec_enable[1].mask = 0;
				} else if (get_current_menunavi()->m == &menu_scanlines) {
					render_osd_page();
				}
				break;
			case RC_LM_MODE:
				strncpy(menu_row1, "Linemult mode:", LCD_ROW_LEN+1);
				strncpy(menu_row2, "press 1-6", LCD_ROW_LEN+1);
				osd_set_menu_active(1);
				ui_disp_menu(1);

				while (1)
				{
					uint32_t btn_vec = SC->controls.ir_code;

					int i;
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
				osd_set_menu_active(0);
				ui_disp_status(0);
				break;
			case RC_PHASE_MINUS:
			case RC_PHASE_PLUS:
				if (phase_hotkey_enable) {
					if (code == RC_PHASE_MINUS)
						video_modes_plm[cm.id].sampler_phase = video_modes_plm[cm.id].sampler_phase ? (video_modes_plm[cm.id].sampler_phase - 1) : SAMPLER_PHASE_MAX;
					else
						video_modes_plm[cm.id].sampler_phase = (video_modes_plm[cm.id].sampler_phase < SAMPLER_PHASE_MAX) ? (video_modes_plm[cm.id].sampler_phase + 1) : 0;

					if (cm.id == vm_edit)
						tc_sampler_phase = video_modes_plm[cm.id].sampler_phase;

					set_sampler_phase(video_modes_plm[cm.id].sampler_phase, 1);

					if (!menu_active) {
						strncpy((char*)OSD->osd_array.data[0][0], menu_advtiming.items[10].name, OSD_CHAR_COLS);
						sampler_phase_disp(video_modes_plm[cm.id].sampler_phase);
						strncpy((char*)OSD->osd_array.data[1][0], menu_row2, OSD_CHAR_COLS);
						osd_status_refresh();
						OSD->osd_row_color.mask = 0;
						OSD->osd_sec_enable[0].mask = 3;
						OSD->osd_sec_enable[1].mask = 0;
					} else if (get_current_menunavi()->m == &menu_advtiming) {
						render_osd_page();
					}
				}
				break;
			case RC_PROF_HOTKEY:
Prof_Hotkey_Prompt:
			{
				strncpy(menu_row1, "Profile load:", LCD_ROW_LEN+1);
				sniprintf(menu_row2, LCD_ROW_LEN+1, "press %u-%u", prof_x10*10, ((prof_x10*10+9) > MAX_PROFILE) ? MAX_PROFILE : (prof_x10*10+9));
				osd_set_menu_active(1);
				ui_disp_menu(1);

				uint32_t btn_vec_prev = 1;
				while (1)
				{
					uint32_t btn_vec = SC->controls.ir_code;

					if ((btn_vec_prev == 0) && (btn_vec != 0)) {
						int i;
						for (i = RC_BTN1; i < REMOTE_MAX_KEYS; i++) {
							if (btn_vec == rc_keymap[i])
								break;
						}

						if ((i == RC_BTN0) || (i < (RC_BTN1 + (prof_x10 == (MAX_PROFILE/10)) ? (MAX_PROFILE%10) : 9))) {
							profile_sel_menu = prof_x10*10 + ((i+1)%10);
							int retval = load_profile();
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
				osd_set_menu_active(0);
				ui_disp_status(0);
				break;
			}
			case RC_RIGHT:
				if (!menu_active)
					man_target_input = next_input;
				break;
			case FP_BTN1:
				man_target_input = next_input;
				break;
			case FP_BTN2:
				tc.sl_mode = tc.sl_mode < SL_MODE_MAX ? tc.sl_mode + 1 : 0;
		}

		if (man_target_input != AV_LAST) {
			target_input = man_target_input;
			ret = 1;
		}
	}

    return ret;
}
