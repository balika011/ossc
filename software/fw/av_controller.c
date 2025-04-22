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

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "system.h"
#include "i2c_opencores.h"
#include "av_controller.h"
#include "tvp7002.h"
#include "ths7353.h"
#include "pcm1862.h"
#include "video_modes.h"
#include "lcd.h"
#include "flash.h"
#include "sdcard.h"
#include "menu.h"
#include "avconfig.h"
#include "userdata.h"
#include "it6613.h"
#include "it6613_sys.h"
#include "HDMI_TX.h"
#include "hdmitx.h"
#include "timer.h"

#define MIN_LINES_PROGRESSIVE   200
#define MIN_LINES_INTERLACED    400
#define STATUS_TIMEOUT_US       25000
#define MAINLOOP_INTERVAL_US    10000

#define PCNT_TOLERANCE 50
#define HSYNC_WIDTH_TOLERANCE 8

// Current mode
avmode_t cm;

tvp_input_t target_tvp;
tvp_sync_input_t target_tvp_sync;
uint8_t target_type;
uint8_t update_cur_vm;

uint8_t profile_sel, profile_sel_menu, input_profiles[AV_LAST], lt_sel, def_input, profile_link, lcd_bl_timeout;
uint8_t osd_enable=1, osd_status_timeout=1;
uint8_t auto_input, auto_av1_ypbpr, auto_av2_ypbpr = 1, auto_av3_ypbpr;

char row1[LCD_ROW_LEN+1], row2[LCD_ROW_LEN+1], menu_row1[LCD_ROW_LEN+1], menu_row2[LCD_ROW_LEN+1];

avinput_t target_input;

uint8_t pcm1862_active;

uint8_t sl_def_iv_x, sl_def_iv_y;

uint32_t read_it2(uint32_t regaddr);

mode_data_t vmode_in, vmode_out;
vm_proc_config_t vm_conf;

uint8_t in_standby;

// Manually (see cyiv-51005.pdf) or automatically (MIF/HEX from PLL megafunction) generated config may not
// provide fully correct scan chain data (e.g. mismatches in C3) and lead to incorrect PLL configuration.
// To get correct scan chain data, do the following:
//   1. Create a ALTPLL_RECONFIG instance with initial value read from your MIF/HEX file
//   2. Connect ALTPLL_RECONFIG to your PLL and set its reconfig input to something you can control easily (e.g. button)
//   3. Create a signaltap file and add all PLL signals to capture. Set sample depth to 256 and clock to scanclk
//   4. Compile the design and program the FPGA
//   5. Open signaltap and set trigger to scanclkena rising edge
//   6. Run signaltap and trigger PLL reconfiguration
//   7. Export VCD file for analysis
//   8. Compare your MIF/HEX to the captured scan chain and update it accordingly
//   9. Dump the updated scan chain data to an array like below (last 16 bits are 0)
//  10. PLL can be then reconfigured with custom pll_reconfig as shown in program_mode()
const pll_config_t pll_configs[] = { {{0x0d806000, 0x00402010, 0x08800020, 0x00080002, 0x00000000}},    // 1x (default)
                                     {{0x0d806000, 0x00402008, 0x04800020, 0x00080002, 0x00000000}},    // 2x (~20-40MHz)
                                     {{0x0d806000, 0x00441c07, 0x02800020, 0x00080002, 0x00000000}},    // 3x (~20-40MHz)
                                     {{0x0d806000, 0x00402004, 0x02800020, 0x00080002, 0x00000000}},    // 4x (~20-40MHz)
                                     {{0x0d806000, 0x00441c05, 0x01800020, 0x00080002, 0x00000000}},    // 5x (~20-40MHz)
                                     {{0x0d806000, 0x00301802, 0x01800020, 0x00080002, 0x00000000}},    // 6x (~20-40MHz)
                                     {{0x0e406000, 0x00281407, 0x02800020, 0x00080002, 0x00000000}},    // 2x (~75MHz)
                                     {{0x04004070, 0x21E0F018, 0x0C800020, 0x00080002, 0x00000000}},    // 1x lo-bw (default)
                                     {{0x04004070, 0x21E0F00C, 0x06800020, 0x00080002, 0x00000000}},    // 2x lo-bw (~20-40MHz)
                                     {{0x04004070, 0x21E0F008, 0x04800020, 0x00080002, 0x00000000}},    // 3x lo-bw (~20-40MHz)
                                     {{0x04004070, 0x21E0F006, 0x03800020, 0x00080002, 0x00000000}},    // 4x lo-bw (~20-40MHz)
                                     {{0x04004070, 0x2190C804, 0x02800020, 0x00080002, 0x00000000}},    // 5x lo-bw (~20-40MHz)
                                     {{0x04004070, 0x21E0F004, 0x02800020, 0x00080002, 0x00000000}},    // 6x lo-bw (~20-40MHz)
                                     {{0x040040F0, 0x61A0D004, 0x02800020, 0x00080002, 0x00000000}} };  // 2x lo-bw (~75MHz)

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
	if (!menu_active) {
        if (refresh_osd_timer)
            OSD->osd_config.status_refresh = 1;

        strncpy((char*)OSD->osd_array.data[0][0], row1, OSD_CHAR_COLS);
        strncpy((char*)OSD->osd_array.data[1][0], row2, OSD_CHAR_COLS);
        OSD->osd_row_color.mask = 0;
        OSD->osd_sec_enable[0].mask = 3;
        OSD->osd_sec_enable[1].mask = 0;

		lcd_write(row1, row2);
	}
}

inline void SetupAudio(tx_mode_t mode)
{
    // shut down audio-tx before setting new config (recommended for changing audio-tx config)
    DisableAudioOutput();
    EnableAudioInfoFrame(FALSE, NULL);

    if (mode != TX_DVI) {
        EnableAudioOutput4OSSC(cm.pclk_o_hz, tc.audio_dw_sampl, tc.audio_swap_lr);
        HDMITX_SetAudioInfoFrame((BYTE)tc.audio_dw_sampl);
#ifdef DEBUG
        Switch_HDMITX_Bank(1);
        usleep(1000);
        uint32_t cts = 0;
        cts |= read_it2(0x35) >> 4;
        cts |= read_it2(0x36) << 4;
        cts |= read_it2(0x37) << 12;
        printf("CTS: %lu\n", cts);
        Switch_HDMITX_Bank(0);
#endif
    }
}

inline void TX_enable(tx_mode_t mode)
{
    // shut down TX before setting new config
    SetAVMute(TRUE);
    DisableVideoOutput();
    EnableAVIInfoFrame(FALSE, NULL);

    //Setup TX configuration
    //TODO: set pclk target and VIC dynamically
    EnableVideoOutput(cm.hdmitx_pclk_level ? PCLK_HIGH : PCLK_MEDIUM, COLOR_RGB444, (mode == TX_HDMI_YCBCR444) ? COLOR_YUV444 : COLOR_RGB444, (mode != TX_DVI));

    if (mode != TX_DVI) {
        HDMITX_SetAVIInfoFrame(vmode_out.vic, (mode == TX_HDMI_RGB) ? F_MODE_RGB444 : F_MODE_YUV444, 0, 0, tc.hdmi_itc, vm_conf.hdmitx_pixr_ifr);
        HDMITX_SetHDRInfoFrame(tc.hdmi_hdr ? 3 : 0);
        HDMITX_SetVRRInfoFrame(tc.hdmi_vrr);
        cm.cc.hdmi_itc = tc.hdmi_itc;
        cm.cc.hdmi_hdr = tc.hdmi_hdr;
        cm.cc.hdmi_vrr = tc.hdmi_vrr;
    }

    SetupAudio(mode);

    // start TX
    SetAVMute(FALSE);
}

int pll_reconfigure(uint8_t mult, uint32_t pclk_i_hz, uint8_t bwsel)
{
    uint8_t id;
	if (!((mult > 1) || ((bwsel == 1) && (pclk_i_hz < 40000000UL)) || (cm.avinput == AV_TESTPAT)))
		id = 0;
	else if ((mult == 2) && (pclk_i_hz > 50000000UL))
		id = ((bwsel+1)*7) - 1;
	else
		id = (bwsel*7) + (mult-1);

	PLL_RECONFIG->pll_config_status.reset = 0;

	if ((id < sizeof(pll_configs)/sizeof(pll_config_t)) && (id != PLL_RECONFIG->pll_config_status.c_config_id)) {
		memcpy((void*)PLL_RECONFIG->pll_config_data.data, pll_configs[id].data, sizeof(pll_config_t));
		PLL_RECONFIG->pll_config_status.t_config_id = id;

		printf("Reconfiguring FPGA PLL to config %u\n", id);

		// Try switching to fixed reference clock as otherwise reconfig may hang or corrupt configuration
		if (cm.avinput != AV_TESTPAT) {
			SC->sys_ctrl.enable_sc = 0;
			usleep(10);
		}

		// Do not reconfigure if clock switch failed
		if (!SC->controls.pll_activeclk) {
			// reset state machine if previous reconfigure hanged (should not occur with stable refclk)
			if (PLL_RECONFIG->pll_config_status.busy) {
				PLL_RECONFIG->pll_config_status.reset = 1;
				usleep(1);
			}

			PLL_RECONFIG->pll_config_status.reset = 0;
			PLL_RECONFIG->pll_config_status.update = 1;
			usleep(10);
		}

		if (cm.avinput != AV_TESTPAT)
			SC->sys_ctrl.enable_sc = 1;

		return 1;
	}

    return 0;
}

void set_lpf(uint8_t lpf)
{
    uint32_t pclk;
    pclk = estimate_dotclk(&vmode_in, (TVP_EXTCLK_HZ/cm.clkcnt));

    //Auto
    if (lpf == 0) {
        if (target_tvp == TVP_INPUT3) {
            tvp_set_lpf((pclk < 30000000) ? 0x0F : 0);
            ths_set_lpf(THS_LPF_BYPASS);
        } else {
            tvp_set_lpf(0);

            switch (target_type) {
            case VIDEO_PC:
            case VIDEO_HDTV:
                ths_set_lpf((pclk < 80000000) ? THS_LPF_35MHZ : THS_LPF_BYPASS);
                break;
            case VIDEO_EDTV:
                ths_set_lpf(THS_LPF_16MHZ);
                break;
            case VIDEO_SDTV:
            default:
                ths_set_lpf(THS_LPF_9MHZ);
                break;
            }
        }
    } else {
        if (target_tvp == TVP_INPUT3) {
            tvp_set_lpf((lpf == 2) ? 0x0F : 0);
            ths_set_lpf(THS_LPF_BYPASS);
        } else {
            tvp_set_lpf(0);
            ths_set_lpf((lpf > 2) ? (VIDEO_LPF_MAX-lpf) : THS_LPF_BYPASS);
        }
    }
}

void set_csc(uint8_t csc)
{
    if (csc > 1) {
        if (target_type == VIDEO_HDTV)
            csc = 1;
        else
            csc = 0;
    }

    tvp_sel_csc(&csc_coeffs[csc]);
}

inline int check_linecnt(uint8_t progressive, uint32_t totlines) {
    if (progressive)
        return (totlines >= MIN_LINES_PROGRESSIVE);
    else
        return (totlines >= MIN_LINES_INTERLACED);
}

// Check if input video status / target configuration has changed
status_t get_status(tvp_sync_input_t syncinput)
{
    uint32_t totlines, clkcnt, pcnt_field;
    uint8_t progressive, sync_active, valid_mode, hsync_width;
    status_t status = NO_CHANGE;
	uint64_t start_ts = timer_timestamp();

	// Wait until vsync active (avoid noise coupled to I2C bus on earlier prototypes)
	while (timer_timestamp() < start_ts + STATUS_TIMEOUT_US)
	{
		if (SC->controls.vsync_flag)
			break;
	}

	// Read sync information from TVP7002 frontend
    sync_active = SC->fe_status.sync_active;
    totlines = SC->fe_status.vtotal;
    progressive = !SC->fe_status.interlace_flag;
    pcnt_field = (unsigned long)SC->fe_status2.pcnt_field;
    hsync_width = (unsigned long)SC->fe_status2.hsync_width;

    clkcnt = pcnt_field/(totlines>>!progressive);
    valid_mode = (pcnt_field > 0) && check_linecnt(progressive, totlines);

    // Check sync activity
    if (!cm.sync_active && sync_active && valid_mode) {
        cm.sync_active = 1;
        status = ACTIVITY_CHANGE;
    } else if (cm.sync_active && (!sync_active || !valid_mode)) {
        cm.sync_active = 0;
        status = ACTIVITY_CHANGE;
    }

    if (sync_active && valid_mode) {
        if ((totlines != cm.totlines) ||
            (progressive != cm.progressive) ||
            (pcnt_field < (cm.pcnt_field - PCNT_TOLERANCE)) ||
            (pcnt_field > (cm.pcnt_field + PCNT_TOLERANCE)) ||
            (abs(((int)hsync_width - (int)cm.hsync_width)) > HSYNC_WIDTH_TOLERANCE)) {
            printf("totlines: %lu (cur) / %lu (prev), pcnt_field: %lu (cur) / %lu (prev), hsync_width: %lu (cur) / %lu (prev)\n", totlines, cm.totlines, pcnt_field, cm.pcnt_field, hsync_width, cm.hsync_width);

            status = (status < MODE_CHANGE) ? MODE_CHANGE : status;
        }

        if (memcmp(&tc, &cm.cc, offsetof(avconfig_t, sl_mode)) || (update_cur_vm == 1))
            status = (status < MODE_CHANGE) ? MODE_CHANGE : status;

        cm.totlines = totlines;
        cm.clkcnt = clkcnt;
        cm.pcnt_field = pcnt_field;
        cm.hsync_width = hsync_width;
        cm.progressive = progressive;
    }

    if (memcmp(&tc.sl_mode, &cm.cc.sl_mode, offsetof(avconfig_t, sync_vth) - offsetof(avconfig_t, sl_mode)))
        status = (status < SC_CONFIG_CHANGE) ? SC_CONFIG_CHANGE : status;

    if (tc.sync_vth != cm.cc.sync_vth)
        tvp_set_sog_thold(tc.sync_vth);

    if (tc.linelen_tol != cm.cc.linelen_tol)
        tvp_set_linelen_tol(tc.linelen_tol);

    if (tc.vsync_thold != cm.cc.vsync_thold)
        tvp_set_ssthold(tc.vsync_thold);

    if ((tc.pre_coast != cm.cc.pre_coast) || (tc.post_coast != cm.cc.post_coast))
        tvp_set_hpllcoast(tc.pre_coast, tc.post_coast);

    if (tc.ypbpr_cs != cm.cc.ypbpr_cs)
        set_csc(tc.ypbpr_cs);

    if (tc.video_lpf != cm.cc.video_lpf)
        set_lpf(tc.video_lpf);

    if (tc.sync_lpf != cm.cc.sync_lpf)
        tvp_set_sync_lpf(tc.sync_lpf);

    if (tc.stc_lpf != cm.cc.stc_lpf)
        tvp_set_clp_lpf(tc.stc_lpf);

    if ((tc.alc_h_filter != cm.cc.alc_h_filter) || (tc.alc_v_filter != cm.cc.alc_v_filter))
        tvp_set_alcfilt(tc.alc_v_filter, tc.alc_h_filter);

    if (memcmp(&tc.col, &cm.cc.col, sizeof(color_setup_t)))
        tvp_set_gain_offset(&tc.col);

    if ((tc.audio_dw_sampl != cm.cc.audio_dw_sampl) ||
#ifdef MANUAL_CTS
        update_cur_vm ||
#endif
        (tc.audio_swap_lr != cm.cc.audio_swap_lr))
        SetupAudio(tc.tx_mode);

    if (pcm1862_active && (tc.audio_gain != cm.cc.audio_gain))
        pcm_set_gain(tc.audio_gain-AUDIO_GAIN_0DB);

    if (pcm1862_active && (tc.audio_mono != cm.cc.audio_mono)) {
        DisableAudioOutput();
        pcm_set_stereo_mode(tc.audio_mono);
        SetupAudio(cm.cc.tx_mode);
    }

    cm.cc = tc;
    update_cur_vm = 0;

    return status;
}

void update_sc_config(mode_data_t *vm_in, mode_data_t *vm_out, vm_proc_config_t *vm_conf, avconfig_t *avconfig)
{
    int i;

    hv_config_reg hv_in_config = {.data=0x00000000};
    hv_config2_reg hv_in_config2 = {.data=0x00000000};
    hv_config3_reg hv_in_config3 = {.data=0x00000000};
    hv_config_reg hv_out_config = {.data=0x00000000};
    hv_config2_reg hv_out_config2 = {.data=0x00000000};
    hv_config3_reg hv_out_config3 = {.data=0x00000000};
    xy_config_reg xy_out_config = {.data=0x00000000};
    xy_config2_reg xy_out_config2 = {.data=0x00000000};
    misc_config_reg misc_config = {.data=0x00000000};
    sl_config_reg sl_config = {.data=0x00000000};
    sl_config2_reg sl_config2 = {.data=0x00000000};
    sl_config3_reg sl_config3 = {.data=0x00000000};

    // Set input params
    hv_in_config.h_total = vm_in->timings.h_total;
    hv_in_config.h_active = vm_in->timings.h_active;
    hv_in_config.h_synclen = vm_in->timings.h_synclen;
    hv_in_config2.h_backporch = vm_in->timings.h_backporch;
    hv_in_config2.v_active = vm_in->timings.v_active;
    hv_in_config3.v_synclen = vm_in->timings.v_synclen;
    hv_in_config3.v_backporch = vm_in->timings.v_backporch;
    hv_in_config2.interlaced = vm_in->timings.interlaced;
    hv_in_config3.v_startline = vm_in->timings.v_synclen+vm_in->timings.v_backporch+12;
    hv_in_config3.h_skip = vm_conf->h_skip;
    hv_in_config3.h_sample_sel = vm_conf->h_sample_sel;

    // Set output params
    hv_out_config.h_total = vm_out->timings.h_total;
    hv_out_config.h_active = vm_out->timings.h_active;
    hv_out_config.h_synclen = vm_out->timings.h_synclen;
    hv_out_config2.h_backporch = vm_out->timings.h_backporch;
    hv_out_config2.v_total = vm_out->timings.v_total;
    hv_out_config2.v_active = vm_out->timings.v_active;
    hv_out_config3.v_synclen = vm_out->timings.v_synclen;
    hv_out_config3.v_backporch = vm_out->timings.v_backporch;
    hv_out_config2.interlaced = vm_out->timings.interlaced;
    hv_out_config3.v_startline = vm_conf->framesync_line;

    xy_out_config.x_size = vm_conf->x_size;
    xy_out_config.y_size = vm_conf->y_size;
    xy_out_config.y_offset = vm_conf->y_offset;
    xy_out_config2.x_offset = vm_conf->x_offset;
    xy_out_config2.x_start_lb = vm_conf->x_start_lb;
    xy_out_config2.y_start_lb = vm_conf->y_start_lb;
    xy_out_config2.x_rpt = vm_conf->x_rpt;
    xy_out_config2.y_rpt = vm_conf->y_rpt;

    misc_config.mask_br = avconfig->mask_br;
    misc_config.mask_color = avconfig->mask_color;
    misc_config.reverse_lpf = avconfig->reverse_lpf;
    misc_config.shmask_mode = avconfig->shmask_mode;
    misc_config.lumacode_mode = avconfig->lumacode_mode;
    /*misc_config.lm_deint_mode = 0;
    misc_config.nir_even_offset = 0;
    misc_config.ypbpr_cs = (avconfig->ypbpr_cs == 0) ? ((vm_in->type & VIDEO_HDTV) ? 1 : 0) : avconfig->ypbpr_cs-1;
    misc_config.vip_enable = 0;
    misc_config.bfi_enable = 0;
    misc_config.bfi_str = 0;*/

    // set default/custom scanline interval
    sl_def_iv_y = (vm_conf->y_rpt > 0) ? vm_conf->y_rpt : 1;
    sl_def_iv_x = (vm_conf->x_rpt > 0) ? vm_conf->x_rpt : sl_def_iv_y;
    sl_config3.sl_iv_x = ((avconfig->sl_type == 3) && (avconfig->sl_cust_iv_x)) ? avconfig->sl_cust_iv_x : sl_def_iv_x;
    sl_config3.sl_iv_y = ((avconfig->sl_type == 3) && (avconfig->sl_cust_iv_y)) ? avconfig->sl_cust_iv_y : sl_def_iv_y;

    // construct custom/default scanline overlay
    for (i=0; i<6; i++) {
        if (avconfig->sl_type == 3) {
            sl_config.sl_l_str_arr |= ((avconfig->sl_cust_l_str[i]-1)&0xf)<<(4*i);
            sl_config.sl_l_overlay |= (avconfig->sl_cust_l_str[i]!=0)<<i;
        } else {
            sl_config.sl_l_str_arr |= avconfig->sl_str<<(4*i);

            if ((i==5) && ((avconfig->sl_type == 0) || (avconfig->sl_type == 2))) {
                sl_config.sl_l_overlay = (1<<((sl_config3.sl_iv_y+1)/2))-1;
                if (avconfig->sl_id)
                    sl_config.sl_l_overlay <<= (sl_config3.sl_iv_y+2)/2;
            }
        }
    }
    for (i = 0; i < 10; i++) {
        if (avconfig->sl_type == 3) {
            if (i < 8)
                sl_config2.sl_c_str_arr_l |= ((avconfig->sl_cust_c_str[i]-1)&0xf)<<(4*i);
            else
                sl_config3.sl_c_str_arr_h |= ((avconfig->sl_cust_c_str[i]-1)&0xf)<<(4*(i-8));
            sl_config3.sl_c_overlay |= (avconfig->sl_cust_c_str[i]!=0)<<i;
        } else {
            if (i < 8)
                sl_config2.sl_c_str_arr_l |= avconfig->sl_str<<(4*i);
            else
                sl_config3.sl_c_str_arr_h |= avconfig->sl_str<<(4*(i-8));

            if ((i==9) && ((avconfig->sl_type == 1) || (avconfig->sl_type == 2)))
                sl_config3.sl_c_overlay = (1<<((sl_config3.sl_iv_x+1)/2))-1;
        }
    }
    sl_config.sl_method = avconfig->sl_method;
    sl_config.sl_altern = avconfig->sl_altern;
    sl_config3.sl_hybr_str = avconfig->sl_hybr_str;

    // disable scanlines if configured so
    if (((avconfig->sl_mode == 1) && (!vm_conf->y_rpt)) || (avconfig->sl_mode == 0)) {
        sl_config.sl_l_overlay = 0;
        sl_config3.sl_c_overlay = 0;
    }

    SC->hv_in_config = hv_in_config;
    SC->hv_in_config2 = hv_in_config2;
    SC->hv_in_config3 = hv_in_config3;
    SC->hv_out_config = hv_out_config;
    SC->hv_out_config2 = hv_out_config2;
    SC->hv_out_config3 = hv_out_config3;
    SC->xy_out_config = xy_out_config;
    SC->xy_out_config2 = xy_out_config2;
    SC->misc_config = misc_config;
    SC->sl_config = sl_config;
    SC->sl_config2 = sl_config2;
    SC->sl_config3 = sl_config3;
}

// Configure TVP7002 and scan converter logic based on the video mode
void program_mode()
{
    int retval, fpga_pll_config_changed, vmode_changed;
    uint8_t h_syncinlen, v_syncinlen, macrovis, hdmitx_pclk_level, osd_x_size, osd_y_size;
    uint32_t h_hz, h_synclen_px, pclk_i_hz, dotclk_hz, pll_h_total;

    memset(&vmode_in, 0, sizeof(mode_data_t));

    vmode_in.timings.v_hz_x100 = (100*TVP_EXTCLK_HZ)/cm.pcnt_field;
    h_hz = (100*TVP_EXTCLK_HZ)/((100*cm.pcnt_field*(1+!cm.progressive))/cm.totlines);

    printf("\nLines: %u %c\n", (unsigned)cm.totlines, cm.progressive ? 'p' : 'i');
    printf("Clocks per line: %u\n", (unsigned)cm.clkcnt);

    //h_syncinlen = tvp_readreg(TVP_HSINWIDTH);
    h_syncinlen = cm.hsync_width;
#ifdef DEBUG
    v_syncinlen = tvp_readreg(TVP_VSINWIDTH);
    macrovis = !!(tvp_readreg(TVP_LINECNT2) & (1<<6));
    printf("Hswidth: %u  Vswidth: %u  Macrovision: %u\n", (unsigned)h_syncinlen, (unsigned)(v_syncinlen & 0x1F), (unsigned)macrovis);
#endif

    vmode_in.timings.h_synclen = h_syncinlen;
    vmode_in.timings.v_total = cm.totlines;
    vmode_in.timings.interlaced = !cm.progressive;

    sniprintf(row1, LCD_ROW_LEN+1, "%s %u-%c", avinput_str[cm.avinput], (unsigned)cm.totlines, cm.progressive ? 'p' : 'i');
    sniprintf(row2, LCD_ROW_LEN+1, "%u.%.2ukHz %u.%.2uHz", (unsigned)(h_hz/1000), (unsigned)((h_hz%1000)/10), (unsigned)(vmode_in.timings.v_hz_x100/100), (unsigned)(vmode_in.timings.v_hz_x100%100));
    ui_disp_status(1);

    retval = get_pure_lm_mode(&cm.cc, &vmode_in, &vmode_out, &vm_conf);

    if (retval == -1) {
        printf ("ERROR: no suitable mode preset found\n");
        vm_conf.si_pclk_mult = 0;
        return;
    }
    vmode_changed = !(cm.id == retval);
    cm.id = retval;
    vm_sel = cm.id;

    pll_h_total = (vm_conf.h_skip+1) * vmode_in.timings.h_total + (((vm_conf.h_skip+1) * vmode_in.timings.h_total_adj * 5 + 50) / 100);

    // Double TVP7002 PLL sampling rate when possible to minimize jitter
    while (1) {
        pclk_i_hz = h_hz * pll_h_total;

        if ((pclk_i_hz < 25000000UL) && ((vm_conf.si_pclk_mult % 2) == 0)) {
            pll_h_total *= 2;
            vm_conf.h_skip = 2*(vm_conf.h_skip+1)-1;
            vm_conf.si_pclk_mult /= 2;
        } else {
            break;
        }
    }

    // Tweak infoframe pixel repetition indicator if passing thru horizontally multiplied mode
    if ((vm_conf.y_rpt == 0) && (vm_conf.h_skip > 0))
        vm_conf.hdmitx_pixr_ifr = vm_conf.h_skip;

    dotclk_hz = estimate_dotclk(&vmode_in, h_hz);
    cm.pclk_o_hz = calculate_pclk(pclk_i_hz, &vmode_out, &vm_conf);

    printf("H: %lu.%.2lukHz V: %u.%.2uHz\n", (h_hz+5)/1000, ((h_hz+5)%1000)/10, (vmode_in.timings.v_hz_x100/100), (vmode_in.timings.v_hz_x100%100));
    printf("Estimated source dot clock: %lu.%.2luMHz\n", (dotclk_hz+5000)/1000000, ((dotclk_hz+5000)%1000000)/10000);
    printf("PCLK_IN: %luHz PCLK_OUT: %luHz\n", pclk_i_hz, cm.pclk_o_hz);

    // Trilevel sync is used with HDTV modes using composite sync
    // CEA-770.3 HDTV modes use tri-level syncs which have twice the width of bi-level syncs of corresponding CEA-861 modes
    if (video_modes_plm[cm.id].type & VIDEO_HDTV) {
        target_type = (target_tvp_sync <= TVP_SOG3) ? VIDEO_HDTV : VIDEO_PC;
        if (target_type == VIDEO_HDTV)
            vmode_in.timings.h_synclen *= 2;
    } else {
        target_type = video_modes_plm[cm.id].type;
    }

    h_synclen_px = ((uint32_t)h_syncinlen * pll_h_total) / cm.clkcnt;

    printf("Mode %s selected - hsync width: %upx\n", video_modes_plm[cm.id].name, (unsigned)h_synclen_px);

    tvp_source_setup(target_type,
                     pll_h_total,
                     (cm.cc.adc_pll_bw == 0) ? vmode_in.timings.h_total : pll_h_total<<(cm.cc.adc_pll_bw-1),
                     cm.clkcnt,
                     cm.cc.tvp_hpll2x && (pclk_i_hz < 50000000UL),
                     (uint8_t)h_synclen_px,
                     (int8_t)(cm.cc.clamp_offset-SIGNED_NUMVAL_ZERO),
                     vmode_changed);
    set_lpf(cm.cc.video_lpf);
    set_csc(cm.cc.ypbpr_cs);

    set_sampler_phase(video_modes_plm[cm.id].sampler_phase, 0);

    fpga_pll_config_changed = pll_reconfigure(vm_conf.si_pclk_mult, pclk_i_hz, cm.cc.fpga_pll_bw);

    update_osd_size(&vmode_out);

    update_sc_config(&vmode_in, &vmode_out, &vm_conf, &cm.cc);

    TX_SetPixelRepetition(vm_conf.tx_pixelrep, ((cm.cc.tx_mode!=TX_DVI) && (vm_conf.tx_pixelrep == vm_conf.hdmitx_pixr_ifr)) ? 1 : 0);

    if (cm.pclk_o_hz > 85000000)
        hdmitx_pclk_level = 1;
    else if (cm.pclk_o_hz < 75000000)
        hdmitx_pclk_level = 0;
    else
        hdmitx_pclk_level = cm.hdmitx_pclk_level;

    printf("PCLK level: %u, PR: %u, IPR: %u, ITC: %u\n", hdmitx_pclk_level, vm_conf.tx_pixelrep, vm_conf.hdmitx_pixr_ifr, cm.cc.hdmi_itc);

    // Full TX initialization increases mode switch delay, use only when necessary
    if (cm.cc.full_tx_setup || fpga_pll_config_changed || (cm.hdmitx_pclk_level != hdmitx_pclk_level)) {
        cm.hdmitx_pclk_level = hdmitx_pclk_level;
        TX_enable(cm.cc.tx_mode);
    } else if (cm.cc.tx_mode!=TX_DVI) {
        HDMITX_SetAVIInfoFrame(vmode_out.vic, (cm.cc.tx_mode == TX_HDMI_RGB) ? F_MODE_RGB444 : F_MODE_YUV444, 0, 0, cm.cc.hdmi_itc, vm_conf.hdmitx_pixr_ifr);
#ifdef MANUAL_CTS
        SetupAudio(cm.cc.tx_mode);
#endif
    }
}

void set_sampler_phase(uint8_t sampler_phase, uint8_t update_sc) {
    uint32_t sample_rng_x1000;
    uint8_t tvp_phase;

    vmode_in.sampler_phase = sampler_phase;

    if (vm_conf.h_skip == 0) {
        vm_conf.h_sample_sel = 0;
        tvp_phase = sampler_phase;
    } else {
        sample_rng_x1000 = 360000 / (vm_conf.h_skip+1);
        vm_conf.h_sample_sel = (sampler_phase*11250)/sample_rng_x1000;
        tvp_phase = ((((sampler_phase*11250) % sample_rng_x1000)*32)/sample_rng_x1000);
    }

    if (vm_conf.h_skip > 0)
        printf("Sample sel: %u/%u\n", (vm_conf.h_sample_sel+1), (vm_conf.h_skip+1));

    tvp_set_hpll_phase(tvp_phase);

    if (update_sc)
        update_sc_config(&vmode_in, &vmode_out, &vm_conf, &cm.cc);
}

int load_profile() {
    int retval;

	retval = userdata_load_profile(profile_sel_menu, 0);
	if (retval == 0) {
        profile_sel = profile_sel_menu;

        // Change the input if the new profile demands it.
        if (tc.link_av != AV_LAST)
            target_input = tc.link_av;

        // Update profile link (also prevents the change of input from inducing a profile load).
        input_profiles[profile_link ? target_input : AV_TESTPAT] = profile_sel;
		userdata_save_initconfig();
	}

    return retval;
}

int save_profile() {
    int retval;

	retval = userdata_save_profile(profile_sel_menu);
	if (retval == 0) {
        profile_sel = profile_sel_menu;

        input_profiles[profile_link ? cm.avinput : AV_TESTPAT] = profile_sel;
		userdata_save_initconfig();
	}

    return retval;
}

// Initialize hardware
int init_hw()
{
	SC->sys_ctrl.lcd_bl_on = 1;
	SC->sys_ctrl.lcd_cs_n = 1;

	// Reset hardware
	SC->sys_ctrl.av_reset_n = 0;
	SC->hv_in_config.data = 0;
    SC->hv_in_config2.data = 0;
	SC->hv_in_config3.data = 0;
	usleep(10000);

    // unreset hw
	SC->sys_ctrl.av_reset_n = 1;

	// Reload initial PLL config (needed after jtagm_reset_req if config has changed).
    // Note that test pattern gets restored only if pclk was active before jtagm_reset_req assertion.
	pll_reconfigure(1, TVP_EXTCLK_HZ, 0);

	//wait >500ms for SD card interface to be stable
    //over 200ms and LCD may be buggy?
	usleep(200000);

	// IT6613 officially supports only 100kHz, but 400kHz seems to work
	I2C_init(I2CA_BASE, ALT_CPU_FREQ, 400000);

	/* Initialize the character display */
	lcd_init();

	if (!SC->controls.is_1_8)
	{
		if (lcd_has_sh1107())
			SC->sys_ctrl.remap_lcd_bl = 1;
		else
			SC->sys_ctrl.remap_red_r = 1;
	}

	strcpy(row1, "      OSSC");
	char fwver[LCD_ROW_LEN + 1];
#if FW_VER_BETA > 0
	sniprintf(fwver, sizeof(fwver), "%u.%.2u beta %u" FW_SUFFIX, FW_VER_MAJOR, FW_VER_MINOR, FW_VER_BETA);
#else
	sniprintf(fwver, sizeof(fwver), "%u.%.2u" FW_SUFFIX, FW_VER_MAJOR, FW_VER_MINOR);
#endif
	// put the fw version into the center
	char *prow2 = row2;
	for (int i = 0; i < (LCD_ROW_LEN - strlen(fwver)) / 2; i++)
		*prow2++ = ' ';
	strcpy(prow2, fwver);
	ui_disp_status(1);

	if (!ths_init())
	{
        printf("Error: could not read from THS7353\n");
        return -2;
    }

    /* check if TVP is found */
	uint32_t chiprev = tvp_readreg(TVP_CHIPREV);
    if (chiprev == 0xff)
	{
        printf("Error: could not read from TVP7002\n");
        return -3;
	}

	tvp_init();

    chiprev = HDMITX_ReadI2C_Byte(IT_DEVICEID);
    if (chiprev != 0x13)
	{
        printf("Error: could not read from IT6613\n");
        return -4;
	}

	InitIT6613();

    if (pcm1862_init()) {
        printf("PCM1862 found\n");
        pcm1862_active = 1;
	}

	sdcard_init();

	// Setup test pattern
    get_vmode(VMODE_480p, &vmode_in, &vmode_out, &vm_conf);
	update_sc_config(&vmode_in, &vmode_out, &vm_conf, &cm.cc);

	// init always in HDMI mode (fixes yellow screen bug)
	TX_enable(TX_HDMI_RGB);

	return 0;
}

void print_vm_stats() {
    int row = 0;

    if (!menu_active) {
        memset((void*)OSD->osd_array.data, 0, sizeof(osd_char_array));
		userdata_load_profile(profile_sel, 1);

		sniprintf((char*)OSD->osd_array.data[row][0], OSD_CHAR_COLS, "Mode preset:");
        sniprintf((char*)OSD->osd_array.data[row][1], OSD_CHAR_COLS, "%s", vmode_out.name);
        sniprintf((char*)OSD->osd_array.data[++row][0], OSD_CHAR_COLS, "Refresh rate:");
        sniprintf((char*)OSD->osd_array.data[row][1], OSD_CHAR_COLS, "%u.%.2uHz", vmode_out.timings.v_hz_x100/100, vmode_out.timings.v_hz_x100%100);
        sniprintf((char*)OSD->osd_array.data[++row][0], OSD_CHAR_COLS, "H/V synclen:");
        sniprintf((char*)OSD->osd_array.data[row][1], OSD_CHAR_COLS, "%-5u %-5u", vmode_out.timings.h_synclen, vmode_out.timings.v_synclen);
        sniprintf((char*)OSD->osd_array.data[++row][0], OSD_CHAR_COLS, "H/V backporch:");
        sniprintf((char*)OSD->osd_array.data[row][1], OSD_CHAR_COLS, "%-5u %-5u", vmode_out.timings.h_backporch, vmode_out.timings.v_backporch);
        sniprintf((char*)OSD->osd_array.data[++row][0], OSD_CHAR_COLS, "H/V active:");
        sniprintf((char*)OSD->osd_array.data[row][1], OSD_CHAR_COLS, "%-5u %-5u", vmode_out.timings.h_active, vmode_out.timings.v_active);
        sniprintf((char*)OSD->osd_array.data[++row][0], OSD_CHAR_COLS, "H/V total:");
        sniprintf((char*)OSD->osd_array.data[row][1], OSD_CHAR_COLS, "%-5u %-5u", vmode_out.timings.h_total, vmode_out.timings.v_total);
        row++;

        sniprintf((char*)OSD->osd_array.data[++row][0], OSD_CHAR_COLS, "Profile:");
        sniprintf((char*)OSD->osd_array.data[row][1], OSD_CHAR_COLS, "%u: %s", profile_sel, (target_profile_name[0] == 0) ? "<empty>" : target_profile_name);
        sniprintf((char*)OSD->osd_array.data[++row][0], OSD_CHAR_COLS, "FW:");
#if FW_VER_BETA > 0
		sniprintf((char *)OSD->osd_array.data[row][1], OSD_CHAR_COLS, "%u.%.2u beta %u" FW_SUFFIX " @ " __DATE__, FW_VER_MAJOR, FW_VER_MINOR, FW_VER_BETA);
#else
		sniprintf((char*)OSD->osd_array.data[row][1], OSD_CHAR_COLS, "%u.%.2u" FW_SUFFIX " @ " __DATE__, FW_VER_MAJOR, FW_VER_MINOR);
#endif

        OSD->osd_config.status_refresh = 1;
        OSD->osd_row_color.mask = 0;
        OSD->osd_sec_enable[0].mask = (1<<(row+1))-1;
        OSD->osd_sec_enable[1].mask = (1<<(row+1))-1;
    }
}

void enter_standby()
{
	in_standby = 1;
	SC->sys_ctrl.lcd_bl_on = 0;
	SC->sys_ctrl.led_g = 0;
	SC->sys_ctrl.led_r = 1;
	SC->sys_ctrl.av_reset_n = 0;
}

void exit_standby()
{
	in_standby = 0;
	SC->sys_ctrl.lcd_bl_on = 1;
	SC->sys_ctrl.led_g = 1;
	SC->sys_ctrl.led_r = 0;
	init_hw();
}

void __attribute__((interrupt, noinline, __section__(".rtext"))) default_exc_handler(void)
{
#if 0
	puts("EXCEPTION!!!\n");
	puts("============\n");
	puts("MEPC:   0x");
	puthex(get_mepc());
	puts("\nMCAUSE: 0x");
	puthex(get_mcause());
	puts("\nMTVAL:  0x");
	puthex(get_mtval());
	putchar('\n');
#endif

	while (1)
		;
}

int main()
{
	timer_init();

	SC->sys_ctrl.led_g = 1;
	SC->sys_ctrl.led_r = 0;

	// Set defaults
	avconfig_set_default();
	controls_set_default();

	memcpy(&cm.cc, &tc, sizeof(avconfig_t));

	// Init menu
	init_menu();

	// Load initconfig and profile
	userdata_load_initconfig();
	userdata_load_profile(profile_sel, 0);

	controls_update();
	uint8_t enter_controls_setup = btn2;

	int init_stat = init_hw();
	if (init_stat < 0)
	{
		sniprintf(row1, sizeof(row1), "Init error  %d", init_stat);
		strncpy(row2, "", sizeof(row2));
		ui_disp_status(1);
		while (1);
	}
	printf("### DIY VIDEO DIGITIZER / SCANCONVERTER INIT OK ###\n\n");

	uint64_t start_ts = timer_timestamp();
	while (timer_timestamp() < start_ts + 500000)
	{
		controls_update();
		enter_controls_setup |= btn2;
	}

	if (enter_controls_setup)
		controls_setup();

	// Mainloop
	uint64_t auto_input_timestamp = 0;
	uint8_t auto_input_changed = 0;
	uint8_t auto_input_ctr = 0;
	uint8_t auto_input_current_ctr = AUTO_CURRENT_MAX_COUNT;
	uint8_t auto_input_keep_current = 0;
	while(1)
	{
		uint64_t start_ts = timer_timestamp();

		controls_update();

		if (!in_standby)
		{
			// Auto input switching
			if ((auto_input != AUTO_OFF) && (cm.avinput != AV_TESTPAT) && !cm.sync_active && !menu_active && (timer_timestamp() >= auto_input_timestamp + 300) && (auto_input_ctr < AUTO_MAX_COUNT))
			{
				// Keep switching on the same physical input when set to Current input or a short time after losing sync.
				auto_input_keep_current = (auto_input == AUTO_CURRENT_INPUT || auto_input_current_ctr < AUTO_CURRENT_MAX_COUNT);

				switch(cm.avinput) {
				case AV1_RGBs:
					target_input = auto_av1_ypbpr ? AV1_YPBPR : AV1_RGsB;
					break;
				case AV1_RGsB:
				case AV1_YPBPR:
					target_input = auto_input_keep_current ? AV1_RGBs : (auto_av2_ypbpr ? AV2_YPBPR : AV2_RGsB);
					break;
				case AV2_YPBPR:
				case AV2_RGsB:
					target_input = auto_input_keep_current ? target_input : AV3_RGBHV;
					break;
				case AV3_RGBHV:
					target_input = AV3_RGBs;
					break;
				case AV3_RGBs:
					target_input = auto_av3_ypbpr ? AV3_YPBPR : AV3_RGsB;
					break;
				case AV3_RGsB:
				case AV3_YPBPR:
					target_input = auto_input_keep_current ? AV3_RGBHV : AV1_RGBs;
					break;
				}

				auto_input_ctr++;

				if (auto_input_current_ctr < AUTO_CURRENT_MAX_COUNT)
					auto_input_current_ctr++;

				// For input linked profile loading below
				auto_input_changed = 1;

				// set auto_input_timestamp
				auto_input_timestamp = timer_timestamp();
			}
		}

		int man_input_change = controls_parse();

		if (!in_standby)
		{
			if (menu_active)
				display_menu(0);

			// Only auto load profile when input is manually changed or when sync is active after automatic switch.
			if ((target_input != cm.avinput && man_input_change) || (auto_input_changed && cm.sync_active))  {
				// The input changed, so load the appropriate profile if
				// input->profile link is enabled
				if (profile_link && (profile_sel != input_profiles[target_input])) {
					profile_sel = input_profiles[target_input];
					userdata_load_profile(profile_sel, 0);
				}

				auto_input_changed = 0;
			}

			if ((target_input != cm.avinput) || ((target_tvp_sync >= TVP_HV_A) && ((tc.av3_alt_rgb != cm.cc.av3_alt_rgb))))
			{
				ths_input_t target_ths;
				pcm_input_t target_pcm;

				target_tvp = TVP_INPUT1;
				target_tvp_sync = TVP_SOG1;

				if ((target_input <= AV1_YPBPR) || (tc.av3_alt_rgb==1 && ((target_input == AV3_RGBHV) || (target_input == AV3_RGBs)))) {
					target_ths = THS_INPUT_B;
					target_pcm = PCM_INPUT4;
				} else if ((target_input <= AV2_RGsB) || (tc.av3_alt_rgb==2 && ((target_input == AV3_RGBHV) || (target_input == AV3_RGBs)))) {
					target_ths = THS_INPUT_A;
					target_pcm = PCM_INPUT3;
				} else  { // if (target_input <= AV3_YPBPR) {
					target_tvp = TVP_INPUT3;
					target_ths = THS_STANDBY;
					target_pcm = PCM_INPUT2;
				}

				video_format target_format;
				switch (target_input) {
				case AV1_RGBs:
				case AV3_RGBs:
					target_format = FORMAT_RGBS;
					break;
				case AV1_RGsB:
				case AV2_RGsB:
				case AV3_RGsB:
					target_format = FORMAT_RGsB;
					break;
				case AV1_YPBPR:
				case AV2_YPBPR:
				case AV3_YPBPR:
					target_format = FORMAT_YPbPr;
					break;
				case AV3_RGBHV:
					target_format = FORMAT_RGBHV;
					break;
				}

				switch (target_input) {
				case AV1_RGBs:
					target_tvp_sync = TVP_SOG2;
					break;
				case AV3_RGBHV:
					target_tvp_sync = TVP_HV_A;
					break;
				case AV3_RGBs:
					target_tvp_sync = TVP_CS_A;
					break;
				case AV3_RGsB:
				case AV3_YPBPR:
					target_tvp_sync = TVP_SOG3;
					break;
				default:
					break;
				}

				printf("### SWITCH MODE TO %s ###\n", avinput_str[target_input]);

				cm.avinput = target_input;
				cm.sync_active = 0;
				ths_source_sel(target_ths, (cm.cc.video_lpf > 1) ? (VIDEO_LPF_MAX-cm.cc.video_lpf) : THS_LPF_BYPASS);
				tvp_powerdown();
				DisableAudioOutput();
				if (pcm1862_active)
					pcm_source_sel(target_pcm);
				tvp_source_sel(target_tvp, target_tvp_sync, target_format);
				cm.clkcnt = 0; //TODO: proper invalidate
				SC->sys_ctrl.vsync_type = target_format == FORMAT_RGBHV;
				strncpy(row1, avinput_str[cm.avinput], LCD_ROW_LEN+1);
				strncpy(row2, "    NO SYNC", LCD_ROW_LEN+1);
				ui_disp_status(1);
				if (man_input_change) {
					// record last input if it was selected manually
					if (def_input == AV_LAST)
						userdata_save_initconfig();
					// Set auto_input_timestamp when input is manually changed
					auto_input_ctr = 0;
					auto_input_timestamp = timer_timestamp();
				}
				// Avoid detection of initial vsync pulses after auto mode switch
				if (auto_input_changed)
					usleep(120000);
			}

			// Check here to enable regardless of input
			if (tc.tx_mode != cm.cc.tx_mode) {
				HDMITX_SetAVIInfoFrame(vmode_out.vic, F_MODE_RGB444, 0, 0, 0, 0);
				TX_enable(tc.tx_mode);
				cm.cc.tx_mode = tc.tx_mode;
				cm.clkcnt = 0; //TODO: proper invalidate
			}
			if (tc.tx_mode != TX_DVI) {
				if (tc.hdmi_itc != cm.cc.hdmi_itc) {
					//EnableAVIInfoFrame(FALSE, NULL);
					printf("setting ITC to %d\n", tc.hdmi_itc);
					HDMITX_SetAVIInfoFrame(vmode_out.vic, (tc.tx_mode == TX_HDMI_RGB) ? F_MODE_RGB444 : F_MODE_YUV444, 0, 0, tc.hdmi_itc, vm_conf.hdmitx_pixr_ifr);
					cm.cc.hdmi_itc = tc.hdmi_itc;
				}
				if (tc.hdmi_hdr != cm.cc.hdmi_hdr) {
					printf("setting HDR flag to %d\n", tc.hdmi_hdr);
					HDMITX_SetHDRInfoFrame(tc.hdmi_hdr ? 3 : 0);
					cm.cc.hdmi_hdr = tc.hdmi_hdr;
				}
				if (tc.hdmi_vrr != cm.cc.hdmi_vrr) {
					printf("setting VRR flag to %d\n", tc.hdmi_vrr);
					HDMITX_SetVRRInfoFrame(tc.hdmi_vrr);
					cm.cc.hdmi_vrr = tc.hdmi_vrr;
				}
			}
			if (tc.av3_alt_rgb != cm.cc.av3_alt_rgb) {
				printf("Changing AV3 RGB source\n");
				cm.cc.av3_alt_rgb = tc.av3_alt_rgb;
			}
			if ((!!osd_enable != OSD->osd_config.enable) || (osd_status_timeout != OSD->osd_config.status_timeout)) {
				OSD->osd_config.enable = !!osd_enable;
				OSD->osd_config.status_timeout = osd_status_timeout;
				if (menu_active) {
					remote_code = 0;
					render_osd_page();
					display_menu(1);
				}
			}

			if (cm.avinput != AV_TESTPAT)
			{
				status_t status = get_status(target_tvp_sync);

				switch (status) {
				case ACTIVITY_CHANGE:
					if (cm.sync_active) {
						printf("Sync up\n");
						SC->sys_ctrl.enable_sc = 1;
						tvp_powerup();
						program_mode();
						SetupAudio(cm.cc.tx_mode);
					} else {
						printf("Sync lost\n");
						cm.clkcnt = 0; //TODO: proper invalidate
						tvp_powerdown();
						//ths_source_sel(THS_STANDBY, 0);
						strncpy(row1, avinput_str[cm.avinput], LCD_ROW_LEN+1);
						strncpy(row2, "    NO SYNC", LCD_ROW_LEN+1);
						ui_disp_status(1);
						// Set auto_input_timestamp
						auto_input_timestamp = timer_timestamp();
						auto_input_ctr = 0;
						auto_input_current_ctr = 0;
					}
					break;
				case MODE_CHANGE:
					if (cm.sync_active) {
						printf("Mode change\n");
						program_mode();
					}
					break;
				case SC_CONFIG_CHANGE:
					if (cm.sync_active) {
						printf("Scanconverter config change\n");
						update_sc_config(&vmode_in, &vmode_out, &vm_conf, &cm.cc);
					}
					break;
				}
			}
		}

		while (timer_timestamp() < start_ts + MAINLOOP_INTERVAL_US)
			;
	}

    return 0;
}
