//
// Copyright (C) 2015-2023  Markus Hiienkari <mhiienka@niksula.hut.fi>
// Copyright (C) 2025       Balázs Triszka   <info@balika011.hu>
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
#include <stdio.h>
#include "menu.h"
#include "system.h"
#include "av_controller.h"
#include "userdata.h"
#include "controls.h"
#include "lcd.h"
#include "tvp7002.h"
#include "firmware.h"
#include "osd.h"

static void vm_select();
static void vm_tweak(uint16_t *v, char *row);

#define SETTING_ITEM(x) 0, sizeof(x)/sizeof(char*)-1, x
#define MENU(X, Y) menuitem_t X##_items[] = Y; static const menu_t X = { sizeof(X##_items)/sizeof(menuitem_t), X##_items };
#define P99_PROTECT(...) __VA_ARGS__

typedef enum
{
    NO_ACTION    = 0,
    OPT_SELECT   = RC_OK,
    PREV_MENU    = RC_BACK,
    PREV_PAGE    = RC_UP,
    NEXT_PAGE    = RC_DOWN,
    VAL_MINUS    = RC_LEFT,
    VAL_PLUS     = RC_RIGHT,
} menucode_id; // order must be consequential with rc_code_t

#define OPT_NOWRAP	0
#define OPT_WRAP	1

uint16_t tc_h_samplerate, tc_h_samplerate_adj, tc_h_synclen, tc_h_bporch, tc_h_active, tc_v_synclen, tc_v_bporch, tc_v_active, tc_sampler_phase, tc_h_ar, tc_v_ar;
uint8_t menu_active;
uint8_t vm_sel, vm_edit;

static const char *off_on_desc[] = { LNG("Off","ｵﾌ"), LNG("On","ｵﾝ") };
static const char *video_lpf_desc[] = { LNG("Auto","ｵｰﾄ"), LNG("Off","ｵﾌ"), "95MHz (HDTV II)", "35MHz (HDTV I)", "16MHz (EDTV)", "9MHz (SDTV)" };
static const char *ypbpr_cs_desc[] = { "Rec. 601", "Rec. 709", "Auto" };
static const char *s480p_mode_desc[] = { LNG("Auto","ｵｰﾄ"), "DTV 480p", "VESA 640x480@60", "PSP 480x272" };
static const char *s400p_mode_desc[] = { "VGA 640x400@70", "VGA 720x400@70" };
static const char *sync_lpf_desc[] = { LNG("2.5MHz (max)","2.5MHz (ｻｲﾀﾞｲ)"), LNG("10MHz (med)","10MHz (ﾁｭｳｲ)"), LNG("33MHz (min)","33MHz (ｻｲｼｮｳ)"), LNG("Off","ｵﾌ") };
static const char *stc_lpf_desc[] = { "4.8MHz (HDTV/PC)", "0.5MHz (SDTV)", "1.7MHz (EDTV)" };
static const char *pt_mode_desc[] = {"Normal", "High samplerate", LNG("512x240 optim.","512x240 ｻｲﾃｷｶ."), LNG("384x240 optim.","384x240 ｻｲﾃｷｶ."), LNG("320x240 optim.","320x240 ｻｲﾃｷｶ."), LNG("256x240 optim.","256x240 ｻｲﾃｷｶ.") };
static const char *l3_mode_desc[] = { LNG("Generic 16:9","ｼﾞｪﾈﾘｯｸ 16:9"), LNG("Generic 4:3","ｼﾞｪﾈﾘｯｸ 4:3"), LNG("512x240 optim.","512x240 ｻｲﾃｷｶ."), LNG("384x240 optim.","384x240 ｻｲﾃｷｶ."), LNG("320x240 optim.","320x240 ｻｲﾃｷｶ."), LNG("256x240 optim.","256x240 ｻｲﾃｷｶ.") };
static const char *l2l4l5l6_mode_desc[] = { LNG("Generic 4:3","ｼﾞｪﾈﾘｯｸ 4:3"), LNG("512x240 optim.","512x240 ｻｲﾃｷｶ."), LNG("384x240 optim.","384x240 ｻｲﾃｷｶ."), LNG("320x240 optim.","320x240 ｻｲﾃｷｶ."), LNG("256x240 optim.","256x240 ｻｲﾃｷｶ.") };
static const char *l5_fmt_desc[] = { "1920x1080", "1600x1200", "1920x1200" };
static const char *pm_240p_desc[] = { LNG("Passthru","ﾊﾟｽｽﾙｰ"), "Line2x", "Line3x", "Line4x", "Line5x", "Line6x" };
static const char *pm_480i_desc[] = { LNG("Passthru","ﾊﾟｽｽﾙｰ"), "Line2x (bob)", "Line3x (laced)", "Line4x (bob)" };
static const char *pm_384p_desc[] = { LNG("Passthru","ﾊﾟｽｽﾙｰ"), "Line2x", "Line3x Generic", "Line2x 240x360", "Line3x 240x360" };
static const char *pm_480p_desc[] = { LNG("Passthru","ﾊﾟｽｽﾙｰ"), "Line2x", "Line3x Generic" };
static const char *pm_1080i_desc[] = { LNG("Passthru","ﾊﾟｽｽﾙｰ"), "Line2x (bob)" };
static const char *ar_256col_desc[] = { "Pseudo 4:3 DAR", "1:1 PAR" };
static const char *tx_mode_desc[] = { "HDMI (RGB)", "HDMI (YCbCr444)", "DVI" };
static const char *sl_mode_desc[] = { LNG("Off","ｵﾌ"), LNG("Auto","ｵｰﾄ"), LNG("On","ｵﾝ") };
static const char *sl_method_desc[] = { LNG("Multiplication","Multiplication"), LNG("Subtraction","Subtraction") };
static const char *sl_type_desc[] = { LNG("Horizontal","ﾖｺ"), LNG("Vertical","ﾀﾃ"), "Horiz. + Vert.", "Custom" };
static const char *sl_id_desc[] = { LNG("Top","ｳｴ"), LNG("Bottom","ｼﾀ") };
static const char *audio_dw_sampl_desc[] = { LNG("Off (fs = 96kHz)","ｵﾌ (fs = 96kHz)"), "2x  (fs = 48kHz)" };
static const char *lt_desc[] = { "Top-left", "Center", "Bottom-right" };
static const char *lcd_bl_timeout_desc[] = { "Off", "3s", "10s", "30s" };
static const char *osd_enable_desc[] = { "Off", "Full", "Simple" };
static const char *osd_status_desc[] = { "2s", "5s", "10s", "Off" };
static const char *rgsb_ypbpr_desc[] = { "RGsB", "YPbPr" };
static const char *auto_input_desc[] = { "Off", "Current input", "All inputs" };
static const char *mask_color_desc[] = { "Black", "Blue", "Green", "Cyan", "Red", "Magenta", "Yellow", "White" };
static const char *av3_alt_rgb_desc[] = { "Off", "AV1", "AV2" };
static const char *shmask_mode_desc[] = { "Off", "A-Grille", "TV", "PVM" };
static const char *lumacode_mode_desc[] = { "Off", "C64", "Spectrum", "Coleco/MSX", "NES" };
static const char *adc_pll_bw_desc[] = { "High", "Medium", "Low", "Ultra low" };
static const char *fpga_pll_bw_desc[] = { "High", "Low" };

static void sync_vth_disp(uint8_t v, char *row) { sniprintf(row, OSD_CHAR_COLS + 1, "%d mV", (v * 1127) / 100); }
static void intclks_to_time_disp(uint8_t v, char *row) { sniprintf(row, OSD_CHAR_COLS + 1, "%u.%.2u us", (unsigned)(((1000000U * v) / (TVP_INTCLK_HZ / 1000)) / 1000), (unsigned)((((1000000U * v) / (TVP_INTCLK_HZ / 1000)) % 1000) / 10)); }
static void extclks_to_time_disp(uint8_t v, char *row) { sniprintf(row, OSD_CHAR_COLS + 1, "%u.%.2u us", (unsigned)(((1000000U * v) / (TVP_EXTCLK_HZ / 1000)) / 1000), (unsigned)((((1000000U * v) / (TVP_EXTCLK_HZ / 1000)) % 1000) / 10)); }
static void sl_str_disp(uint8_t v, char *row) { sniprintf(row, OSD_CHAR_COLS + 1, "%u%%", ((v + 1) * 625) / 100); }
static void sl_cust_str_disp(uint8_t v, char *row) { sniprintf(row, OSD_CHAR_COLS + 1, "%u%%", ((v) * 625) / 100); }
static void sl_cust_iv_x_disp(uint8_t v, char *row) { sniprintf(row, OSD_CHAR_COLS + 1, "%s%u%s", (v ? "" : "Auto ("), (v ? v : sl_def_iv_x) + 1, (v ? "" : ")")); }
static void sl_cust_iv_y_disp(uint8_t v, char *row) { sniprintf(row, OSD_CHAR_COLS + 1, "%s%u%s", (v ? "" : "Auto ("), (v ? v : sl_def_iv_y) + 1, (v ? "" : ")")); }
static void sl_hybr_str_disp(uint8_t v, char *row) { sniprintf(row, OSD_CHAR_COLS + 1, "%u%%", (v * 625) / 100); }
static void lines_disp(uint8_t v, char *row) { sniprintf(row, OSD_CHAR_COLS + 1, LNG("%u lines", "%u ﾗｲﾝ"), v); }
static void pixels_disp(uint8_t v, char *row) { sniprintf(row, OSD_CHAR_COLS + 1, LNG("%u pixels", "%u ﾄﾞｯﾄ"), v); }
static void value_disp(uint8_t v, char *row) { sniprintf(row, OSD_CHAR_COLS + 1, "%u", v); }
static void signed_disp(uint8_t v, char *row) { sniprintf(row, OSD_CHAR_COLS + 1, "%d", (int8_t)(v - SIGNED_NUMVAL_ZERO)); }
static void lt_disp(uint8_t v, char *row) { strncpy(row, lt_desc[v], OSD_CHAR_COLS + 1); }
static void aud_db_disp(uint8_t v, char *row) { sniprintf(row, OSD_CHAR_COLS + 1, "%d dB", ((int8_t)v - AUDIO_GAIN_0DB)); }
static void vm_display_name(uint8_t v, char *row) { strncpy(row, video_modes_plm[v].name, OSD_CHAR_COLS + 1); }
static void link_av_desc(avinput_t v, char *row) { strncpy(row, v == AV_LAST ? "No link" : avinput_str[v], OSD_CHAR_COLS + 1); }
static void profile_disp(uint8_t v, char *row) { userdata_load_profile(v, 1); sniprintf(row, OSD_CHAR_COLS + 1, "%u: %s", v, (target_profile_name[0] == 0) ? "<empty>" : target_profile_name); }
static void alc_v_filter_disp(uint8_t v, char *row) { sniprintf(row, OSD_CHAR_COLS + 1, LNG("%u lines", "%u ﾗｲﾝ"), (1 << v)); }
static void alc_h_filter_disp(uint8_t v, char *row) { sniprintf(row, OSD_CHAR_COLS + 1, LNG("%u pixels", "%u ﾄﾞｯﾄ"), (1 << (v + 1))); }
void sampler_phase_disp(uint8_t v, char *row) { sniprintf(row, OSD_CHAR_COLS + 1, "%d deg", (v * 11250) / 1000); }
// static void coarse_gain_disp(uint8_t v, char *row) { sniprintf(row, OSD_CHAR_COLS + 1, "%u.%u", ((v*10)+50)/100, (((v*10)+50)%100)/10); }

static arg_info_t vm_arg_info = {&vm_sel, 0, vm_display_name};
static const arg_info_t profile_arg_info = {&profile_sel_menu, MAX_PROFILE, profile_disp};
static const arg_info_t lt_arg_info = {&lt_sel, (sizeof(lt_desc)/sizeof(char*))-1, lt_disp};

MENU(menu_advtiming, P99_PROTECT({ \
	{ LNG("H. samplerate","H. ｻﾝﾌﾟﾙﾚｰﾄ"),        OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_samplerate, H_TOTAL_MIN,   H_TOTAL_MAX, vm_tweak } } },
	{ "H. s.rate frac",                           OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_samplerate_adj, 0,  H_TOTAL_ADJ_MAX, vm_tweak } } },
	{ LNG("H. synclen","H. ﾄﾞｳｷﾅｶﾞｻ"),       OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_synclen,    H_SYNCLEN_MIN, H_SYNCLEN_MAX, vm_tweak } } },
	{ LNG("H. backporch","H. ﾊﾞｯｸﾎﾟｰﾁ"),        OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_bporch,     H_BPORCH_MIN,  H_BPORCH_MAX, vm_tweak } } },
	{ LNG("H. active","H. ｱｸﾃｨﾌﾞ"),             OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_active,     H_ACTIVE_MIN,  H_ACTIVE_SMP_MAX, vm_tweak } } },
	{ LNG("V. synclen","V. ﾄﾞｳｷﾅｶﾞｻ"),       OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_v_synclen,    V_SYNCLEN_MIN, V_SYNCLEN_MAX, vm_tweak } } },
	{ LNG("V. backporch","V. ﾊﾞｯｸﾎﾟｰﾁ"),       OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_v_bporch,     V_BPORCH_MIN,  V_BPORCH_MAX, vm_tweak } } },
	{ LNG("V. active","V. ｱｸﾃｨﾌﾞ"),            OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_v_active,     V_ACTIVE_MIN,  V_ACTIVE_MAX, vm_tweak } } },
	{ "H. border",                              OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_ar,    0, H_AR_MAX, vm_tweak } } },
	{ "V. border",                              OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_v_ar,    0, V_AR_MAX, vm_tweak } } },
	{ LNG("Sampling phase","ｻﾝﾌﾟﾘﾝｸﾞﾌｪｰｽﾞ"),     OPT_AVCONFIG_NUMVAL_U16,  { .num_u16 = { &tc_sampler_phase, 0, SAMPLER_PHASE_MAX, vm_tweak } } },
}))

MENU(menu_cust_sl, P99_PROTECT({ \
	{ "H interval",                           OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_iv_x,          OPT_NOWRAP, 0, 4, sl_cust_iv_x_disp } } },
	{ "V interval",                           OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_iv_y,          OPT_NOWRAP, 0, 5, sl_cust_iv_y_disp } } },
	{ "Sub-line 1 str",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_l_str[0],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
	{ "Sub-line 2 str",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_l_str[1],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
	{ "Sub-line 3 str",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_l_str[2],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
	{ "Sub-line 4 str",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_l_str[3],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
	{ "Sub-line 5 str",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_l_str[4],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
	{ "Sub-line 6 str",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_l_str[5],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
	{ "Sub-column 1 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[0],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
	{ "Sub-column 2 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[1],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
	{ "Sub-column 3 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[2],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
	{ "Sub-column 4 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[3],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
	{ "Sub-column 5 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[4],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
	{ "Sub-column 6 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[5],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
}))


MENU(menu_vinputproc, P99_PROTECT({ \
	{ LNG("Video LPF","ﾋﾞﾃﾞｵ LPF"),             OPT_AVCONFIG_SELECTION, { .sel = { &tc.video_lpf,     OPT_WRAP,   SETTING_ITEM(video_lpf_desc) } } },
	{ LNG("Reverse LPF","ｷﾞｬｸLPF"),             OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.reverse_lpf,  OPT_NOWRAP, 0, REVERSE_LPF_MAX, value_disp } } },
	{ LNG("YPbPr in ColSpa","ｲﾛｸｳｶﾝﾆYPbPr"),    OPT_AVCONFIG_SELECTION, { .sel = { &tc.ypbpr_cs,      OPT_WRAP,   SETTING_ITEM(ypbpr_cs_desc) } } },
	{ LNG("R/Pr offset","R/Pr ｵﾌｾｯﾄ"),          OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.col.r_f_off,   OPT_NOWRAP, 0, 0xFF, value_disp } } },
	{ LNG("G/Y offset","G/Y ｵﾌｾｯﾄ"),            OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.col.g_f_off,   OPT_NOWRAP, 0, 0xFF, value_disp } } },
	{ LNG("B/Pb offset","B/Pb ｵﾌｾｯﾄ"),          OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.col.b_f_off,   OPT_NOWRAP, 0, 0xFF, value_disp } } },
	{ LNG("R/Pr gain","R/Pr ｹﾞｲﾝ"),             OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.col.r_f_gain,  OPT_NOWRAP, 0, 0xFF, value_disp } } },
	{ LNG("G/Y gain","G/Y ｹﾞｲﾝ"),               OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.col.g_f_gain,  OPT_NOWRAP, 0, 0xFF, value_disp } } },
	{ LNG("B/Pb gain","B/Pb ｹﾞｲﾝ"),             OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.col.b_f_gain,  OPT_NOWRAP, 0, 0xFF, value_disp } } },
	{ LNG("Pre-ADC Gain","Pre-ADC Gain"),       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.col.c_gain,    OPT_NOWRAP, 0, COARSE_GAIN_MAX, value_disp } } },
	{ "Clamp/ALC offset",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.clamp_offset,  OPT_NOWRAP, CLAMP_OFFSET_MIN, CLAMP_OFFSET_MAX, signed_disp } } },
	{ "ALC V filter",                           OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.alc_v_filter,  OPT_NOWRAP, 0, ALC_V_FILTER_MAX, alc_v_filter_disp } } },
	{ "ALC H filter",                           OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.alc_h_filter,  OPT_NOWRAP, 0, ALC_H_FILTER_MAX, alc_h_filter_disp } } },
	{ "Lumacode",                              OPT_AVCONFIG_SELECTION, { .sel = { &tc.lumacode_mode,  OPT_WRAP,   SETTING_ITEM(lumacode_mode_desc) } } },
}))

MENU(menu_sampling, P99_PROTECT({ \
	{ LNG("480p in sampler","ｻﾝﾌﾟﾗｰﾃﾞ480p"),     OPT_AVCONFIG_SELECTION, { .sel = { &tc.s480p_mode,    OPT_WRAP, SETTING_ITEM(s480p_mode_desc) } } },
	{ LNG("400p in sampler","ｻﾝﾌﾟﾗｰﾃﾞ400p"),     OPT_AVCONFIG_SELECTION, { .sel = { &tc.s400p_mode,    OPT_WRAP, SETTING_ITEM(s400p_mode_desc) } } },
	{ LNG("Allow upsample2x","ｱｯﾌﾟｻﾝﾌﾟﾙ2xｷｮﾖｳ"), OPT_AVCONFIG_SELECTION, { .sel = { &tc.upsample2x,   OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
	{ LNG("Adv. timing    >","ｶｸｼｭﾀｲﾐﾝｸﾞ >"),	OPT_SUBMENU,			{ .sub = { &menu_advtiming, &vm_arg_info, vm_select } } },
}))

MENU(menu_sync, P99_PROTECT({ \
	{ LNG("Analog sync LPF","ｱﾅﾛｸﾞﾄﾞｳｷ LPF"),    OPT_AVCONFIG_SELECTION, { .sel = { &tc.sync_lpf,    OPT_WRAP,   SETTING_ITEM(sync_lpf_desc) } } },
	{ "Analog STC LPF",                         OPT_AVCONFIG_SELECTION, { .sel = { &tc.stc_lpf,    OPT_WRAP,   SETTING_ITEM(stc_lpf_desc) } } },
	{ LNG("Analog sync Vth","ｱﾅﾛｸﾞﾄﾞｳｷ Vth"),    OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sync_vth,    OPT_NOWRAP, 0, SYNC_VTH_MAX, sync_vth_disp } } },
	{ LNG("Hsync tolerance","Hsyncｺｳｻ"),        OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.linelen_tol, OPT_NOWRAP, 0, 0xFF, intclks_to_time_disp } } },
	{ LNG("Vsync threshold","Vsyncｼｷｲﾁ"),       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.vsync_thold, OPT_NOWRAP, VSYNC_THOLD_MIN, VSYNC_THOLD_MAX, intclks_to_time_disp } } },
	{ "H-PLL Pre-Coast",                        OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.pre_coast,   OPT_NOWRAP, 0, PLL_COAST_MAX, lines_disp } } },
	{ "H-PLL Post-Coast",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.post_coast,  OPT_NOWRAP, 0, PLL_COAST_MAX, lines_disp } } },
	{ "ADC PLL BW",                             OPT_AVCONFIG_SELECTION, { .sel = { &tc.adc_pll_bw,  OPT_WRAP,   SETTING_ITEM(adc_pll_bw_desc) } } },
	{ "FPGA PLL BW",                            OPT_AVCONFIG_SELECTION, { .sel = { &tc.fpga_pll_bw, OPT_WRAP,   SETTING_ITEM(fpga_pll_bw_desc) } } },
}))

MENU(menu_output, P99_PROTECT({ \
	{ LNG("240p/288p proc","240p/288pｼｮﾘ"),     OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_240p,         OPT_WRAP, SETTING_ITEM(pm_240p_desc) } } },
	{ LNG("384p/400p proc","384p/400pｼｮﾘ"),     OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_384p,         OPT_WRAP, SETTING_ITEM(pm_384p_desc) } } },
	{ LNG("480i/576i proc","480i/576iｼｮﾘ"),     OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_480i,         OPT_WRAP, SETTING_ITEM(pm_480i_desc) } } },
	{ LNG("480p/576p proc","480p/576pｼｮﾘ"),     OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_480p,         OPT_WRAP, SETTING_ITEM(pm_480p_desc) } } },
	{ LNG("960i/1080i proc","960i/1080iｼｮﾘ"),   OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_1080i,        OPT_WRAP, SETTING_ITEM(pm_1080i_desc) } } },
	{ "Passthru mode",                         OPT_AVCONFIG_SELECTION, { .sel = { &tc.pt_mode,         OPT_WRAP,  SETTING_ITEM(pt_mode_desc) } } },
	{ LNG("Line2x mode","Line2xﾓｰﾄﾞ"),          OPT_AVCONFIG_SELECTION, { .sel = { &tc.l2_mode,         OPT_WRAP, SETTING_ITEM(l2l4l5l6_mode_desc) } } },
	{ LNG("Line3x mode","Line3xﾓｰﾄﾞ"),          OPT_AVCONFIG_SELECTION, { .sel = { &tc.l3_mode,         OPT_WRAP, SETTING_ITEM(l3_mode_desc) } } },
	{ LNG("Line4x mode","Line4xﾓｰﾄﾞ"),          OPT_AVCONFIG_SELECTION, { .sel = { &tc.l4_mode,         OPT_WRAP, SETTING_ITEM(l2l4l5l6_mode_desc) } } },
	{ LNG("Line5x mode","Line5xﾓｰﾄﾞ"),          OPT_AVCONFIG_SELECTION, { .sel = { &tc.l5_mode,         OPT_WRAP, SETTING_ITEM(l2l4l5l6_mode_desc) } } },
	{ LNG("Line6x mode","Line6xﾓｰﾄﾞ"),          OPT_AVCONFIG_SELECTION, { .sel = { &tc.l6_mode,         OPT_WRAP, SETTING_ITEM(l2l4l5l6_mode_desc) } } },
	{ LNG("Line5x format","Line5xｹｲｼｷ"),        OPT_AVCONFIG_SELECTION, { .sel = { &tc.l5_fmt,          OPT_WRAP, SETTING_ITEM(l5_fmt_desc) } } },
	{ LNG("256x240 aspect","256x240ｱｽﾍﾟｸﾄ"),    OPT_AVCONFIG_SELECTION, { .sel = { &tc.ar_256col,       OPT_WRAP, SETTING_ITEM(ar_256col_desc) } } },
	{ LNG("TX mode","TXﾓｰﾄﾞ"),                  OPT_AVCONFIG_SELECTION, { .sel = { &tc.tx_mode,         OPT_WRAP, SETTING_ITEM(tx_mode_desc) } } },
	{ "HDMI ITC",                              OPT_AVCONFIG_SELECTION, { .sel = { &tc.hdmi_itc,        OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
	{ "HDMI HDR flag",                         OPT_AVCONFIG_SELECTION, { .sel = { &tc.hdmi_hdr,        OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
	{ "HDMI VRR flag",                         OPT_AVCONFIG_SELECTION, { .sel = { &tc.hdmi_vrr,        OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
}))

MENU(menu_scanlines, P99_PROTECT({ \
	{ LNG("Scanlines","ｽｷｬﾝﾗｲﾝ"),                  OPT_AVCONFIG_SELECTION, { .sel = { &tc.sl_mode,     OPT_WRAP,   SETTING_ITEM(sl_mode_desc) } } },
	{ LNG("Sl. strength","ｽｷｬﾝﾗｲﾝﾂﾖｻ"),            OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_str,      OPT_NOWRAP, 0, SCANLINESTR_MAX, sl_str_disp } } },
	{ "Sl. hybrid str.",                          OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_hybr_str, OPT_NOWRAP, 0, SL_HYBRIDSTR_MAX, sl_hybr_str_disp } } },
	{ "Sl. method",                               OPT_AVCONFIG_SELECTION, { .sel = { &tc.sl_method,   OPT_WRAP,   SETTING_ITEM(sl_method_desc) } } },
	{ "Sl. alternating",                          OPT_AVCONFIG_SELECTION, { .sel = { &tc.sl_altern,   OPT_WRAP,   SETTING_ITEM(off_on_desc) } } },
	{ LNG("Sl. alignment","ｽｷｬﾝﾗｲﾝﾎﾟｼﾞｼｮﾝ"),        OPT_AVCONFIG_SELECTION, { .sel = { &tc.sl_id,       OPT_WRAP,   SETTING_ITEM(sl_id_desc) } } },
	{ LNG("Sl. type","ｽｷｬﾝﾗｲﾝﾙｲ"),                 OPT_AVCONFIG_SELECTION, { .sel = { &tc.sl_type,     OPT_WRAP,   SETTING_ITEM(sl_type_desc) } } },
	{ "<  Custom Sl.  >",                         OPT_SUBMENU,            { .sub = { &menu_cust_sl, NULL, NULL } } },
}))

MENU(menu_postproc, P99_PROTECT({ \
	{ "Shadow mask",                             OPT_AVCONFIG_SELECTION, { .sel = { &tc.shmask_mode, OPT_WRAP,   SETTING_ITEM(shmask_mode_desc) } } },
	{ "Border color",                            OPT_AVCONFIG_SELECTION, { .sel = { &tc.mask_color,  OPT_NOWRAP,   SETTING_ITEM(mask_color_desc) } } },
	{ LNG("Border brightn.","ﾏｽｸｱｶﾙｻ"),           OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.mask_br,     OPT_NOWRAP, 0, HV_MASK_MAX_BR, value_disp } } },
    //{ LNG("<DIY lat. test>","DIYﾁｴﾝﾃｽﾄ"),         OPT_FUNC_CALL,          { .fun = { latency_test, &lt_arg_info } } },
}))

MENU(menu_compatibility, P99_PROTECT({ \
	{ LNG("Full TX setup","ﾌﾙTXｾｯﾄｱｯﾌﾟ"),         OPT_AVCONFIG_SELECTION, { .sel = { &tc.full_tx_setup,    OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
	{ LNG("Allow TVP HPLL2x","TVP HPLL2xｷｮﾖｳ"), OPT_AVCONFIG_SELECTION, { .sel = { &tc.tvp_hpll2x,   OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
	{ "AV3 use alt. RGB",                        OPT_AVCONFIG_SELECTION, { .sel = { &tc.av3_alt_rgb,     OPT_WRAP, SETTING_ITEM(av3_alt_rgb_desc) } } },
	{ "Default HDMI VIC",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.default_vic,     OPT_NOWRAP, 0, HDMI_1080p50, value_disp } } },
}))

MENU(menu_audio, P99_PROTECT({ \
	{ LNG("Down-sampling","ﾀﾞｳﾝｻﾝﾌﾟﾘﾝｸﾞ"),       OPT_AVCONFIG_SELECTION, { .sel = { &tc.audio_dw_sampl, OPT_WRAP, SETTING_ITEM(audio_dw_sampl_desc) } } },
	{ LNG("Swap left/right","ﾋﾀﾞﾘ/ﾐｷﾞｽﾜｯﾌﾟ"),    OPT_AVCONFIG_SELECTION, { .sel = { &tc.audio_swap_lr,  OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
	{ "Mono mode",                              OPT_AVCONFIG_SELECTION,  { .sel = { &tc.audio_mono,      OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
	{ "Pre-ADC gain",                           OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.audio_gain,     OPT_NOWRAP, 0, AUDIO_GAIN_MAX, aud_db_disp } } },
}))

MENU(menu_settings, P99_PROTECT({ \
	{ LNG("Link prof->input","Link prof->input"), OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.link_av,  OPT_WRAP, AV1_RGBs, AV_LAST, link_av_desc } } },
	{ LNG("Link input->prof","Link input->prof"),   OPT_AVCONFIG_SELECTION, { .sel = { &profile_link,  OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
	{ LNG("Initial input","ｼｮｷﾆｭｳﾘｮｸ"),          OPT_AVCONFIG_SELECTION, { .sel = { &def_input,       OPT_WRAP, SETTING_ITEM(avinput_str) } } },
	{ "Autodetect input",                          OPT_AVCONFIG_SELECTION, { .sel = { &auto_input,     OPT_WRAP, SETTING_ITEM(auto_input_desc) } } },
	{ "Auto AV1 Y/Gs",                          OPT_AVCONFIG_SELECTION, { .sel = { &auto_av1_ypbpr,     OPT_WRAP, SETTING_ITEM(rgsb_ypbpr_desc) } } },
	{ "Auto AV2 Y/Gs",                          OPT_AVCONFIG_SELECTION, { .sel = { &auto_av2_ypbpr,     OPT_WRAP, SETTING_ITEM(rgsb_ypbpr_desc) } } },
	{ "Auto AV3 Y/Gs",                          OPT_AVCONFIG_SELECTION, { .sel = { &auto_av3_ypbpr,     OPT_WRAP, SETTING_ITEM(rgsb_ypbpr_desc) } } },
	{ "LCD BL timeout",                         OPT_AVCONFIG_SELECTION, { .sel = { &lcd_bl_timeout,  OPT_WRAP, SETTING_ITEM(lcd_bl_timeout_desc) } } },
	{ "OSD",                                    OPT_AVCONFIG_SELECTION, { .sel = { &osd_enable,   OPT_WRAP,   SETTING_ITEM(osd_enable_desc) } } },
	{ "OSD status disp.",                       OPT_AVCONFIG_SELECTION, { .sel = { &osd_status_timeout,   OPT_WRAP,   SETTING_ITEM(osd_status_desc) } } },
	{ "Phase hotkey",                           OPT_AVCONFIG_SELECTION, { .sel = { &phase_hotkey_enable,  OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
	{ LNG("<Load profile  >","<ﾌﾟﾛﾌｧｲﾙﾛｰﾄﾞ   >"),   OPT_FUNC_CALL,         { .fun = { load_profile, &profile_arg_info } } },
	{ LNG("<Save profile  >","<ﾌﾟﾛﾌｧｲﾙｾｰﾌﾞ   >"),  OPT_FUNC_CALL,          { .fun = { save_profile, &profile_arg_info } } },
	{ LNG("<Reset settings>","<ｾｯﾃｲｦｼｮｷｶ     >"),  OPT_FUNC_CALL,          { .fun = { avconfig_set_default, NULL } } },
	{ LNG("<Import sett.  >","<ｾｯﾃｲﾖﾐｺﾐ      >"), OPT_FUNC_CALL,        { .fun = { userdata_import, NULL } } },
	{ LNG("<Export sett.  >","<ｾｯﾃｲｶｷｺﾐ      >"), OPT_FUNC_CALL,        { .fun = { userdata_export, NULL } } },
	{ LNG("<Fw. update    >","<ﾌｧｰﾑｳｪｱｱｯﾌﾟﾃﾞｰﾄ>"), OPT_FUNC_CALL,        { .fun = { fw_update, NULL } } },
}))


MENU(menu_main, P99_PROTECT({ \
	{ LNG("Video in proc  >","ﾀｲｵｳｴｲｿﾞｳ     >"),  OPT_SUBMENU,            { .sub = { &menu_vinputproc, NULL, NULL } } },
	{ LNG("Sampling opt.  >","ｻﾝﾌﾟﾘﾝｸﾞｵﾌﾟｼｮﾝ>"),  OPT_SUBMENU,            { .sub = { &menu_sampling, NULL, NULL } } },
	{ LNG("Sync opt.      >","ﾄﾞｳｷｵﾌﾟｼｮﾝ    >"),  OPT_SUBMENU,            { .sub = { &menu_sync, NULL, NULL } } },
	{ LNG("Output opt.    >","ｼｭﾂﾘｮｸｵﾌﾟｼｮﾝ  >"),  OPT_SUBMENU,            { .sub = { &menu_output, NULL, NULL } } },
	{ LNG("Scanline opt.  >","ｽｷｬﾝﾗｲﾝｵﾌﾟｼｮﾝ >"),  OPT_SUBMENU,            { .sub = { &menu_scanlines, NULL, NULL } } },
	{ LNG("Post-proc.     >","ｱﾄｼｮﾘ         >"),  OPT_SUBMENU,            { .sub = { &menu_postproc, NULL, NULL } } },
	{ LNG("Compatibility  >","ｺﾞｶﾝｾｲ        >"),  OPT_SUBMENU,            { .sub = { &menu_compatibility, NULL, NULL } } },
	{ LNG("Audio options  >","ｵｰﾃﾞｨｵｵﾌﾟｼｮﾝ     >"),                  OPT_SUBMENU,            { .sub = { &menu_audio, NULL, NULL } } },
	{ LNG("Settings opt   >","ｾｯﾃｲｶﾝﾘ       >"),  OPT_SUBMENU,            { .sub = { &menu_settings, NULL, NULL } } },
}))

// Max 3 levels currently
menunavi navi[] = {{&menu_main, 0}, {NULL, 0}, {NULL, 0}};
uint8_t navlvl = 0;

void menu_init()
{
	// Set max ids for adv timing
	vm_arg_info.max = VIDEO_MODE_COUNT - 1;

	// Setup OSD
	osd_init();
}

void write_option_value(menuitem_t *item, int func_called, int retval, char *row)
{
	switch (item->type) {
		case OPT_AVCONFIG_SELECTION:
			strncpy(row, item->sel.setting_str[*(item->sel.data)], OSD_CHAR_COLS + 1);
			break;
		case OPT_AVCONFIG_NUMVALUE:
			item->num.df(*(item->num.data), row);
			break;
		case OPT_AVCONFIG_NUMVAL_U16:
			item->num_u16.df(item->num_u16.data, row);
			break;
		case OPT_SUBMENU:
			if (item->sub.arg_info)
				item->sub.arg_info->df(*item->sub.arg_info->data, row);
			else
				row[0] = 0;
			break;
		case OPT_FUNC_CALL:
			if (func_called) {
				if (retval == 0)
					strncpy(row, "Done", OSD_CHAR_COLS + 1);
				else if (retval < 0)
					sniprintf(row, OSD_CHAR_COLS + 1, "Failed (%d)", retval);
			} else if (item->fun.arg_info) {
				item->fun.arg_info->df(*item->fun.arg_info->data, row);
			} else {
				row[0] = 0;
			}
			break;
		default:
			break;
	}
}

void menu_render_row(uint8_t row)
{
	if (!menu_active)
		return;

	if (osd_enable == 0 && row == navi[navlvl].mp)
	{
		menuitem_t *item = &navi[navlvl].m->items[row];
		char row_text[OSD_CHAR_COLS + 1];
		write_option_value(item, 0, 0, row_text);
		lcd_write_row1(item->name);
		lcd_write_row2(row_text);
	}
	else if (osd_enable == 1)
	{
		menuitem_t *item = &navi[navlvl].m->items[row];

		osd_draw_text(row, 0, 1, (row == navi[navlvl].mp) ? 3 : 2, item->name);

		if (row == navi[navlvl].mp)
			lcd_write_row1(item->name);

		if ((item->type != OPT_SUBMENU) && (item->type != OPT_FUNC_CALL))
		{
			char row_text[OSD_CHAR_COLS + 1];
			write_option_value(item, 0, 0, row_text);
			if (row_text[0])
				osd_draw_text(row, 1, 1, (row == navi[navlvl].mp) ? 3 : 2, row_text);

			if (row == navi[navlvl].mp)
				lcd_write_row2(row_text);
		}
		else
		{
			osd_draw_text(row, 1, 0, 0, "");
			lcd_write_row2("");
		}
	}
	else if (osd_enable == 2 && row == navi[navlvl].mp)
	{
		menuitem_t *item = &navi[navlvl].m->items[row];
		char row_text[OSD_CHAR_COLS + 1];
		write_option_value(item, 0, 0, row_text);
		osd_notification(item->name, row_text);
	}
}

void menu_render_page()
{
	if (!menu_active)
		return;

	osd_clear();

	if (osd_enable == 1)
		for (int i = 0; i < navi[navlvl].m->num_items; i++)
			menu_render_row(i);
	else
		menu_render_row(navi[navlvl].mp);
}

void menu_update()
{
	menucode_id code = NO_ACTION;
	menuitem_t *item;
	uint8_t *val, val_wrap, val_min, val_max;
	uint16_t *val_u16, val_u16_min, val_u16_max;
	int i, func_called = 0, retval = 0;

	if (!remote_code)
		return;

	for (i=RC_OK; i < RC_INFO; i++) {
		if (remote_code == rc_keymap[i]) {
			code = i;
			break;
		}
	}

	item = &navi[navlvl].m->items[navi[navlvl].mp];

	// Parse menu control
	switch (code) {
	case PREV_PAGE:
	case NEXT_PAGE:
	{
		uint8_t last_page = navi[navlvl].mp;
		if (code == PREV_PAGE)
			navi[navlvl].mp = (navi[navlvl].mp == 0) ? navi[navlvl].m->num_items - 1 : (navi[navlvl].mp - 1);
		else
			navi[navlvl].mp = (navi[navlvl].mp + 1) % navi[navlvl].m->num_items;
		menu_render_row(last_page);
		break;
	}
	case PREV_MENU:
		if (navlvl > 0) {
			navlvl--;
			menu_render_page();
		} else {
			menu_active = 0;
			osd_set_menu_active(0);
			return;
		}
		break;
	case OPT_SELECT:
		switch (item->type) {
			case OPT_SUBMENU:
				if (item->sub.arg_f)
					item->sub.arg_f();

				if (navi[navlvl+1].m != item->sub.menu)
					navi[navlvl+1].mp = 0;
				navi[navlvl+1].m = item->sub.menu;
				navlvl++;
				menu_render_page();

				break;
			case OPT_FUNC_CALL:
				retval = item->fun.f();
				func_called = 1;
				break;
			default:
				break;
		}
		break;
	case VAL_MINUS:
	case VAL_PLUS:
		switch (item->type) {
			case OPT_AVCONFIG_SELECTION:
			case OPT_AVCONFIG_NUMVALUE:
				val = item->sel.data;
				val_wrap = item->sel.wrap_cfg;
				val_min = item->sel.min;
				val_max = item->sel.max;

				if (code == VAL_MINUS)
					*val = (*val > val_min) ? (*val-1) : (val_wrap ? val_max : val_min);
				else
					*val = (*val < val_max) ? (*val+1) : (val_wrap ? val_min : val_max);
				if (val == &osd_enable)
					menu_render_page();
				break;
			case OPT_AVCONFIG_NUMVAL_U16:
				val_u16 = item->num_u16.data;
				val_u16_min = item->num_u16.min;
				val_u16_max = item->num_u16.max;
				val_wrap = (val_u16_min == 0);
				if (code == VAL_MINUS)
					*val_u16 = (*val_u16 > val_u16_min) ? (*val_u16-1) : (val_wrap ? val_u16_max : val_u16_min);
				else
					*val_u16 = (*val_u16 < val_u16_max) ? (*val_u16+1) : (val_wrap ? val_u16_min : val_u16_max);
				break;
			case OPT_SUBMENU:
				val = item->sub.arg_info->data;
				val_max = item->sub.arg_info->max;

				if (item->sub.arg_info) {
					if (code == VAL_MINUS)
						*val = (*val > 0) ? (*val-1) : 0;
					else
						*val = (*val < val_max) ? (*val+1) : val_max;
				}
				break;
			case OPT_FUNC_CALL:
				val = item->fun.arg_info->data;
				val_max = item->fun.arg_info->max;

				if (item->fun.arg_info) {
					if (code == VAL_MINUS)
						*val = (*val > 0) ? (*val-1) : 0;
					else
						*val = (*val < val_max) ? (*val+1) : val_max;
				}
				break;
			default:
				break;
		}
		break;
	}

	menu_render_row(navi[navlvl].mp);
}

void menu_update_status(const char *str)
{
	if (str[0])
	{
		if (osd_enable == 1)
			osd_draw_text(navi[navlvl].mp, 1, 1, 3, str);
		else if (osd_enable == 2)
			osd_draw_text(1, 0, 1, 2, str);
	}

	lcd_write_row2(str);
}

static void vm_select() {
	vm_edit = vm_sel;
	tc_h_samplerate = video_modes_plm[vm_edit].timings.h_total;
	tc_h_samplerate_adj = (uint16_t)video_modes_plm[vm_edit].timings.h_total_adj;
	tc_h_synclen = (uint16_t)video_modes_plm[vm_edit].timings.h_synclen;
	tc_h_bporch = video_modes_plm[vm_edit].timings.h_backporch;
	tc_h_active = video_modes_plm[vm_edit].timings.h_active;
	tc_v_synclen = (uint16_t)video_modes_plm[vm_edit].timings.v_synclen;
	tc_v_bporch = video_modes_plm[vm_edit].timings.v_backporch;
	tc_v_active = video_modes_plm[vm_edit].timings.v_active;
	tc_h_ar = (uint16_t)video_modes_plm[vm_edit].ar.h;
	tc_v_ar = (uint16_t)video_modes_plm[vm_edit].ar.v;
	tc_sampler_phase = video_modes_plm[vm_edit].sampler_phase;
}

static void vm_tweak(uint16_t *v, char *row)
{
	int active_mode = (cm.sync_active && (cm.id == vm_edit));

	if (active_mode) {
		if ((video_modes_plm[cm.id].timings.h_total != tc_h_samplerate) ||
			(video_modes_plm[cm.id].timings.h_total_adj != (uint8_t)tc_h_samplerate_adj) ||
			(video_modes_plm[cm.id].timings.h_synclen != (uint8_t)tc_h_synclen) ||
			(video_modes_plm[cm.id].timings.h_backporch != tc_h_bporch) ||
			(video_modes_plm[cm.id].timings.h_active != tc_h_active) ||
			(video_modes_plm[cm.id].timings.v_synclen != (uint8_t)tc_v_synclen) ||
			(video_modes_plm[cm.id].timings.v_backporch != tc_v_bporch) ||
			(video_modes_plm[cm.id].timings.v_active != tc_v_active) ||
			(video_modes_plm[cm.id].ar.h != tc_h_ar) ||
			(video_modes_plm[cm.id].ar.v != tc_v_ar))
			update_cur_vm = 1;
		if (video_modes_plm[cm.id].sampler_phase != tc_sampler_phase)
			set_sampler_phase(tc_sampler_phase, 1);
	}
	video_modes_plm[vm_edit].timings.h_total = tc_h_samplerate;
	video_modes_plm[vm_edit].timings.h_total_adj = (uint8_t)tc_h_samplerate_adj;
	video_modes_plm[vm_edit].timings.h_synclen = (uint8_t)tc_h_synclen;
	video_modes_plm[vm_edit].timings.h_backporch = tc_h_bporch;
	video_modes_plm[vm_edit].timings.h_active = tc_h_active;
	video_modes_plm[vm_edit].timings.v_synclen = (uint8_t)tc_v_synclen;
	video_modes_plm[vm_edit].timings.v_backporch = tc_v_bporch;
	video_modes_plm[vm_edit].timings.v_active = tc_v_active;
	video_modes_plm[vm_edit].ar.h = tc_h_ar;
	video_modes_plm[vm_edit].ar.v = tc_v_ar;
	video_modes_plm[vm_edit].sampler_phase = tc_sampler_phase;

	if (v == &tc_sampler_phase)
		sampler_phase_disp(*v, row);
	else if ((v == &tc_h_samplerate) || (v == &tc_h_samplerate_adj))
		sniprintf(row, OSD_CHAR_COLS + 1, "%u.%.2u", video_modes_plm[vm_edit].timings.h_total, video_modes_plm[vm_edit].timings.h_total_adj * 5);
	else
		sniprintf(row, OSD_CHAR_COLS + 1, "%u", *v);
}

void menu_sampler_phase(uint8_t v)
{
	if (!menu_active)
	{
		char row[OSD_CHAR_COLS + 1];
		sampler_phase_disp(v, row);
		osd_status(LNG("Sampling phase", "ｻﾝﾌﾟﾘﾝｸﾞﾌｪｰｽﾞ"), row);
	}
	else if (navi[navlvl].m == &menu_advtiming)
	{
		menu_render_page();
	}
}

void menu_scanlines_mode()
{
	if (!menu_active)
	{
		osd_status(LNG("Scanlines", "ｽｷｬﾝﾗｲﾝ"), sl_mode_desc[tc.sl_mode]);
	}
	else if (navi[navlvl].m == &menu_scanlines)
	{
		menu_render_page();
	}
}

void menu_scanlines_type()
{
	if (!menu_active)
	{
		osd_status(LNG("Sl. type", "ｽｷｬﾝﾗｲﾝﾙｲ"), sl_type_desc[tc.sl_type]);
	}
	else if (navi[navlvl].m == &menu_scanlines)
	{
		menu_render_page();
	}
}

void menu_scanlines_strength()
{
	if (!menu_active)
	{
		char row[OSD_CHAR_COLS + 1];
		sl_str_disp(tc.sl_str, row);
		osd_status(LNG("Sl. strength", "ｽｷｬﾝﾗｲﾝﾂﾖｻ"), row);
	}
	else if (navi[navlvl].m == &menu_scanlines)
	{
		menu_render_page();
	}
}