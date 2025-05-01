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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "system.h"
#include "video_modes.h"
#include "av_controller.h"
#include "avconfig.h"

#define VM_OUT_YMULT        (vm_conf->y_rpt+1)
#define VM_OUT_XMULT        (vm_conf->x_rpt+1)
#define VM_OUT_PCLKMULT     (((vm_conf->x_rpt+1)*(vm_conf->y_rpt+1))/(vm_conf->h_skip+1))

static const mode_data_t video_modes_plm_default[VIDEO_MODE_COUNT] = {
    /* 240p modes */
    { "1600x240",      HDMI_Unknown,     {1600,  240,   6000,  2046, 0,  262,  202, 15,  150, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_SDTV,               GROUP_240P,   (MODE_L5_GEN_4_3),                                                                          },
    { "1280x240",      HDMI_Unknown,     {1280,  240,   6000,  1560, 0,  262,  170, 15,   72, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_SDTV,               GROUP_240P,   (MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                                                       },
    { "960x240",       HDMI_Unknown,     { 960,  240,   6000,  1170, 0,  262,  128, 15,   54, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_SDTV,               GROUP_240P,   (MODE_L3_GEN_4_3 | MODE_L6_GEN_4_3),                                                        },
    { "512x240",       HDMI_Unknown,     { 512,  240,   6000,   682, 0,  262,   77, 14,   50, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_512_COL | MODE_L3_512_COL | MODE_L4_512_COL | MODE_L5_512_COL | MODE_L6_512_COL),  },
    { "384x240",       HDMI_Unknown,     { 384,  240,   6000,   512, 0,  262,   59, 14,   37, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_384_COL | MODE_L3_384_COL | MODE_L4_384_COL | MODE_L5_384_COL | MODE_L6_384_COL),  },
    { "320x240",       HDMI_Unknown,     { 320,  240,   6000,   426, 0,  262,   49, 14,   31, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_320_COL | MODE_L3_320_COL | MODE_L4_320_COL | MODE_L5_320_COL | MODE_L6_320_COL),  },
    { "256x240",       HDMI_Unknown,     { 256,  240,   6000,   341, 0,  262,   39, 14,   25, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_256_COL | MODE_L3_256_COL | MODE_L4_256_COL | MODE_L5_256_COL | MODE_L6_256_COL),  },
    { "240p",          HDMI_240p60,      { 720,  240,   6005,   858, 0,  262,   57, 15,   62, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_SDTV,               GROUP_240P,   (MODE_PT | MODE_L2),                                                                        },
    /* 288p modes */
    { "1280x288",      HDMI_Unknown,     {1280,  288,   5000,  1560, 0,  312,  170, 19,   72, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_SDTV,               GROUP_288P,   (MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3 | MODE_L6_GEN_4_3),                                     },
    { "960x288",       HDMI_Unknown,     { 960,  288,   5000,  1170, 0,  312,  128, 19,   54, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_SDTV,               GROUP_288P,   (MODE_L3_GEN_4_3 | MODE_L5_GEN_4_3),                                                        },
    { "512x240LB",     HDMI_Unknown,     { 512,  240,   5000,   682, 0,  312,   77, 41,   50, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_512_COL | MODE_L3_512_COL | MODE_L4_512_COL | MODE_L5_512_COL | MODE_L6_512_COL),  },
    { "384x240LB",     HDMI_Unknown,     { 384,  240,   5000,   512, 0,  312,   59, 41,   37, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_384_COL | MODE_L3_384_COL | MODE_L4_384_COL | MODE_L5_384_COL | MODE_L6_384_COL),  },
    { "320x240LB",     HDMI_Unknown,     { 320,  240,   5000,   426, 0,  312,   49, 41,   31, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_320_COL | MODE_L3_320_COL | MODE_L4_320_COL | MODE_L5_320_COL | MODE_L6_320_COL),  },
    { "256x240LB",     HDMI_Unknown,     { 256,  240,   5000,   341, 0,  312,   39, 41,   25, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_256_COL | MODE_L3_256_COL | MODE_L4_256_COL | MODE_L5_256_COL | MODE_L6_256_COL),  },
    { "288p",          HDMI_288p50,      { 720,  288,   5008,   864, 0,  312,   69, 19,   63, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_SDTV,               GROUP_288P,   (MODE_PT | MODE_L2),                                                                        },
    /* 360p: GBI */
    { "480x360",       HDMI_Unknown,     { 480,  360,   6000,   600, 0,  375,   63, 10,   38, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_EDTV,               GROUP_384P,   (MODE_PT | MODE_L2),                                                                        },
    { "240x360",       HDMI_Unknown,     { 240,  360,   6000,   300, 0,  375,   32, 10,   18, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_EDTV,               GROUP_384P,   (MODE_L2_240x360 | MODE_L3_240x360),                                                        },
    /* 384p: Sega Model 2 */
    { "384p",          HDMI_Unknown,     { 496,  384,   5500,   640, 0,  423,   50, 29,   62, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_EDTV,               GROUP_384P,   (MODE_PT | MODE_L2),                                                                        },
    /* 400p line3x */
    { "1600x400",      HDMI_Unknown,     {1600,  400,   7000,  2000, 0,  449,  120, 34,  240, 2,  0},  DEF_PHASE,  { 0,  0},  VIDEO_PC,                 GROUP_384P,   (MODE_L3_GEN_16_9),                                                                         },
    /* 720x400@70Hz, VGA Mode 3+/7+ */
    { "720x400_70",    HDMI_Unknown,     { 720,  400,   7000,   900, 0,  449,   64, 34,   96, 2,  0},  DEF_PHASE,  { 0,  0},  VIDEO_PC,                 GROUP_384P,   (MODE_PT | MODE_L2),                                                                        },
    /* 640x400@70Hz, VGA Mode 13h */
    { "640x400_70",    HDMI_Unknown,     { 640,  400,   7000,   800, 0,  449,   48, 34,   96, 2,  0},  DEF_PHASE,  { 0,  0},  VIDEO_PC,                 GROUP_384P,   (MODE_PT | MODE_L2),                                                                        },
    /* 384p: X68k @ 24kHz */
    { "640x384",       HDMI_Unknown,     { 640,  384,   5500,   800, 0,  492,   48, 63,   96, 2,  0},  DEF_PHASE,  { 0,  0},  VIDEO_PC,                 GROUP_384P,   (MODE_PT | MODE_L2),                                                                        },
    /* ~525-line modes */
    { "480i",          HDMI_480i60,      { 720,  240,   5994,   858, 0,  525,   57, 15,   62, 3,  1},  DEF_PHASE,  { 0,  0},  VIDEO_SDTV,               GROUP_480I,   (MODE_PT | MODE_L2 | MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                                   },
    { "1920x480",      HDMI_Unknown,     { 960,  480,   5994,  1287, 0,  525,  150, 30,   93, 6,  0},  DEF_PHASE,  { 0,  0},  VIDEO_EDTV,               GROUP_480P,   (MODE_L3_GEN_16_9),                                                                         },
    { "480p",          HDMI_480p60,      { 720,  480,   5994,   858, 0,  525,   60, 30,   62, 6,  0},  DEF_PHASE,  { 0,  0},  VIDEO_EDTV,               GROUP_480P,   (MODE_PT | MODE_L2),                                                                        },
    { "640x480_60",    HDMI_640x480p60,  { 640,  480,   6000,   800, 0,  525,   48, 33,   96, 2,  0},  DEF_PHASE,  { 0,  0},  VIDEO_PC,                 GROUP_480P,   (MODE_PT | MODE_L2),                                                                        },
     /* 480p PSP in-game */ \
    { "480x272",       HDMI_480p60_16x9, { 480,  272,   6000,   858, 0,  525,  177,134,   62, 6,  0},  DEF_PHASE,  { 0,  0},  VIDEO_EDTV,               GROUP_480P,   (MODE_PT | MODE_L2)                                                                         },                                                                  \
    /* X68k @ 31kHz */
    { "640x512",       HDMI_Unknown,     { 640,  512,   6000,   800, 0,  568,   48, 34,   96, 6,  0},  DEF_PHASE,  { 0,  0},  VIDEO_PC,                 GROUP_480P,   (MODE_PT | MODE_L2),                                                                        },
    /* ~625-line modes */
    { "576i",          HDMI_576i50,      { 720,  288,   5000,   864, 0,  625,   69, 19,   63, 3,  1},  DEF_PHASE,  { 0,  0},  VIDEO_SDTV,               GROUP_576I,   (MODE_PT | MODE_L2 | MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                                   },
    { "1920x480LB",    HDMI_Unknown,     { 960,  480,   5000,  1296, 0,  625,  162, 87,   96, 5,  0},  DEF_PHASE,  { 0,  0},  VIDEO_EDTV,               GROUP_576P,   (MODE_L3_GEN_16_9),                                                                         },
    { "576p",          HDMI_576p50,      { 720,  576,   5000,   864, 0,  625,   68, 39,   64, 5,  0},  DEF_PHASE,  { 0,  0},  VIDEO_EDTV,               GROUP_576P,   (MODE_PT | MODE_L2),                                                                        },
    { "800x600_60",    HDMI_Unknown,     { 800,  600,   6000,  1056, 0,  628,   88, 23,  128, 4,  0},  DEF_PHASE,  { 0,  0},  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                                    },
    /* CEA 720p modes */
    { "720p_50",       HDMI_720p50,      {1280,  720,   5000,  1980, 0,  750,  220, 20,   40, 5,  0},  DEF_PHASE,  { 0,  0},  (VIDEO_HDTV | VIDEO_PC),  GROUP_720P,   MODE_PT,                                                                                    },
    { "720p_60",       HDMI_720p60,      {1280,  720,   6000,  1650, 0,  750,  220, 20,   40, 5,  0},  DEF_PHASE,  { 0,  0},  (VIDEO_HDTV | VIDEO_PC),  GROUP_720P,   MODE_PT,                                                                                    },
    /* VESA XGA,1280x960 and SXGA modes */
    { "1024x768",      HDMI_Unknown,     {1024,  768,   6000,  1344, 0,  806,  160, 29,  136, 6,  0},  DEF_PHASE,  { 0,  0},  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                                    },
    { "1280x960",      HDMI_Unknown,     {1280,  960,   6000,  1800, 0, 1000,  312, 36,  112, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                                    },
    { "1280x1024",     HDMI_Unknown,     {1280, 1024,   6000,  1688, 0, 1066,  248, 38,  112, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                                    },
    /* PS2 GSM 960i mode */
    { "640x960i",      HDMI_Unknown,     { 640,  480,   6000,   800, 0, 1050,   48, 33,   96, 2,  1},  DEF_PHASE,  { 0,  0},  VIDEO_EDTV,               GROUP_1080I,  (MODE_PT | MODE_L2),                                                                        },
    /* CEA 1080i/p modes */
    { "1080i_50",      HDMI_1080i50,     {1920,  540,   5000,  2640, 0, 1125,  148, 15,   44, 5,  1},  DEF_PHASE,  { 0,  0},  (VIDEO_HDTV | VIDEO_PC),  GROUP_1080I,  (MODE_PT | MODE_L2),                                                                        },
    { "1080i_60",      HDMI_1080i60,     {1920,  540,   6000,  2200, 0, 1125,  148, 15,   44, 5,  1},  DEF_PHASE,  { 0,  0},  (VIDEO_HDTV | VIDEO_PC),  GROUP_1080I,  (MODE_PT | MODE_L2),                                                                        },
    { "1080p_50",      HDMI_1080p50,     {1920, 1080,   5000,  2640, 0, 1125,  148, 36,   44, 5,  0},  DEF_PHASE,  { 0,  0},  (VIDEO_HDTV | VIDEO_PC),  GROUP_1080P,  MODE_PT,                                                                                    },
    { "1080p_60",      HDMI_1080p60,     {1920, 1080,   6000,  2200, 0, 1125,  148, 36,   44, 5,  0},  DEF_PHASE,  { 0,  0},  (VIDEO_HDTV | VIDEO_PC),  GROUP_1080P,  MODE_PT,                                                                                    },
    /* VESA UXGA mode */
    { "1600x1200",     HDMI_Unknown,     {1600, 1200,   6000,  2160, 0, 1250,  304, 46,  192, 3,  0},  DEF_PHASE,  { 0,  0},  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                                    },
};

mode_data_t video_modes_plm[VIDEO_MODE_COUNT];

void vm_table_set_default() {
    memcpy(video_modes_plm, video_modes_plm_default, sizeof(video_modes_plm_default));
}

void vmode_hv_mult(mode_data_t *vmode, uint8_t h_mult, uint8_t v_mult) {
    uint32_t val, bp_extra;

    val = vmode->timings.h_synclen * h_mult;
    if (val > H_SYNCLEN_MAX) {
        vmode->timings.h_synclen = H_SYNCLEN_MAX;
        bp_extra = val - vmode->timings.h_synclen;
    } else {
        vmode->timings.h_synclen = val;
        bp_extra = 0;
    }

    val = (vmode->timings.h_backporch * h_mult) + bp_extra;
    if (val > H_BPORCH_MAX)
        vmode->timings.h_backporch = H_BPORCH_MAX;
    else
        vmode->timings.h_backporch = val;

    val = vmode->timings.h_active * h_mult;
    if (val > H_ACTIVE_MAX)
        vmode->timings.h_active = H_ACTIVE_MAX;
    else
        vmode->timings.h_active = val;

    vmode->timings.h_total = h_mult * vmode->timings.h_total + ((h_mult * vmode->timings.h_total_adj * 5 + 50) / 100);

    val = vmode->timings.v_synclen * v_mult;
    if (val > V_SYNCLEN_MAX) {
        vmode->timings.v_synclen = V_SYNCLEN_MAX;
        bp_extra = val - vmode->timings.v_synclen;
    } else {
        vmode->timings.v_synclen = val;
        bp_extra = 0;
    }

    val = (vmode->timings.v_backporch * v_mult) + bp_extra;
    if (val > V_BPORCH_MAX)
        vmode->timings.v_backporch = V_BPORCH_MAX;
    else
        vmode->timings.v_backporch = val;

    val = vmode->timings.v_active * v_mult;
    if (val > V_ACTIVE_MAX)
        vmode->timings.v_active = V_ACTIVE_MAX;
    else
        vmode->timings.v_active = val;

    if (vmode->timings.interlaced && ((v_mult % 2) == 0)) {
        vmode->timings.interlaced = 0;
        vmode->timings.v_total *= (v_mult / 2);
    } else {
        vmode->timings.v_total *= v_mult;
    }
}

uint32_t estimate_dotclk(mode_data_t *vm_in, uint32_t h_hz) {
    if ((vm_in->type & VIDEO_SDTV) ||
        (vm_in->type & VIDEO_EDTV))
    {
        return h_hz * 858;
    } else {
        return vm_in->timings.h_total * h_hz;
    }
}

uint32_t calculate_pclk(uint32_t src_clk_hz, mode_data_t *vm_out, vm_proc_config_t *vm_conf) {
    uint32_t pclk_hz;

    if (vm_conf->si_pclk_mult > 0) {
        pclk_hz = vm_conf->si_pclk_mult*src_clk_hz;
    } else if (vm_conf->si_pclk_mult < 0) {
        pclk_hz = src_clk_hz/((-1)*vm_conf->si_pclk_mult+1);
    } else {
        // Round to kHz but maximize accuracy without using 64-bit division
        pclk_hz = (((uint32_t)((((uint64_t)vm_out->timings.h_total*vm_out->timings.v_total*vm_out->timings.v_hz_x100)>>vm_out->timings.interlaced)/8)+6250)/12500)*1000;

        // Switch to integer mult if possible
        if (!vm_conf->framelock) {
            if ((pclk_hz >= src_clk_hz) && (pclk_hz % src_clk_hz == 0))
                vm_conf->si_pclk_mult = (pclk_hz/src_clk_hz);
            else if ((pclk_hz < src_clk_hz) && (src_clk_hz % pclk_hz == 0))
                vm_conf->si_pclk_mult = (-1)*((src_clk_hz/pclk_hz)-1);
        }
    }

    pclk_hz *= vm_conf->tx_pixelrep+1;

    return pclk_hz;
}

int get_pure_lm_mode(avconfig_t *cc, mode_data_t *vm_in, mode_data_t *vm_out, vm_proc_config_t *vm_conf)
{
    int i, diff_lines, diff_v_hz_x100, mindiff_id=0, mindiff_lines=1000, mindiff_v_hz_x100=10000;
    mode_data_t *mode_preset;
    mode_flags valid_lm[] = { (MODE_PT | (cc->pt_mode ? (MODE_L5_GEN_4_3<<(cc->pt_mode-1)) : 0)),
                              (MODE_L2 | (MODE_L2<<cc->l2_mode)),
                              (MODE_L3_GEN_16_9<<cc->l3_mode),
                              (MODE_L4_GEN_4_3<<cc->l4_mode),
                              (MODE_L5_GEN_4_3<<cc->l5_mode),
                              (MODE_L6_GEN_4_3<<cc->l6_mode) };
    mode_flags target_lm, mindiff_lm;
    uint8_t pt_only = 0;
    uint8_t upsample2x = cc->upsample2x;

    // one for each video_group
    uint8_t* group_ptr[] = { &pt_only, &cc->pm_240p, &cc->pm_240p, &cc->pm_384p, &cc->pm_480i, &cc->pm_480i, &cc->pm_480p, &cc->pm_480p, &pt_only, &cc->pm_1080i, &pt_only };

	for (i = 0; i < VIDEO_MODE_COUNT; i++)
	{
		mode_preset = &video_modes_plm[i];

        switch (mode_preset->group) {
            case GROUP_384P:
                //fixed Line2x/3x mode for 240x360p/400p
                valid_lm[2] = MODE_L3_GEN_16_9;
                valid_lm[3] = MODE_L2_240x360;
                valid_lm[4] = MODE_L3_240x360;
                if ((!vm_in->timings.h_total) && (mode_preset->timings.v_total == 449)) {
                    if ((video_modes_plm_default[i].timings.h_active == 720) && (video_modes_plm_default[i].timings.v_active == 400)) {
                        if (cc->s400p_mode == 0)
                            continue;
                    } else if ((video_modes_plm_default[i].timings.h_active == 640) && (video_modes_plm_default[i].timings.v_active == 400)) {
                        if (cc->s400p_mode == 1)
                            continue;
                    }
                }
                break;
            case GROUP_480I:
            case GROUP_576I:
                //fixed Line3x/4x mode for 480i and onwards
                valid_lm[2] = MODE_L3_GEN_16_9;
                valid_lm[3] = MODE_L4_GEN_4_3;
                break;
            case GROUP_480P:
                 if (mode_preset->vic == HDMI_480p60) {
                    switch (cc->s480p_mode) {
                        case 0: // Auto
                            if (vm_in->timings.h_synclen > 82)
                                continue;
                            break;
                        case 1: // DTV 480p
                            break;
                        default:
                            continue;
                    }
                } else if (mode_preset->vic == HDMI_640x480p60) {
                    switch (cc->s480p_mode) {
                        case 0: // Auto
                        case 2: // VESA 640x480@60
                            break;
                        default:
                            continue;
                    }
                }
                break;
            default:
                break;
        }

        target_lm = valid_lm[*group_ptr[mode_preset->group]];

        if ((target_lm & mode_preset->flags) &&
            (vm_in->timings.interlaced == mode_preset->timings.interlaced))
        {
            diff_lines = abs(vm_in->timings.v_total - mode_preset->timings.v_total);
            diff_v_hz_x100 = abs(vm_in->timings.v_hz_x100 - mode_preset->timings.v_hz_x100);

            if ((diff_lines < mindiff_lines) || ((mode_preset->group >= GROUP_720P) && (diff_lines == mindiff_lines) && (diff_v_hz_x100 < mindiff_v_hz_x100))) {
                mindiff_id = i;
                mindiff_lines = diff_lines;
                mindiff_v_hz_x100 = diff_v_hz_x100;
                mindiff_lm = target_lm;
            } else if ((mindiff_lines <= 2) && (diff_lines > mindiff_lines)) {
                // Break out if suitable mode already found
                break;
            }
        }
	}

	if (mindiff_lines >= 130)
        return -1;

    mode_preset = &video_modes_plm[mindiff_id];

    vm_in->timings.h_active = mode_preset->timings.h_active;
    vm_in->timings.v_active = mode_preset->timings.v_active;
    vm_in->timings.h_synclen = mode_preset->timings.h_synclen;
    vm_in->timings.v_synclen = mode_preset->timings.v_synclen;
    vm_in->timings.h_backporch = mode_preset->timings.h_backporch;
    vm_in->timings.v_backporch = mode_preset->timings.v_backporch;
    vm_in->timings.h_total = mode_preset->timings.h_total;
    vm_in->timings.h_total_adj = mode_preset->timings.h_total_adj;
    vm_in->sampler_phase = mode_preset->sampler_phase;
    vm_in->ar.h = mode_preset->ar.h;
    vm_in->ar.v = mode_preset->ar.v;
    vm_in->type = mode_preset->type;
    vm_in->group = mode_preset->group;
    vm_in->vic = mode_preset->vic;
    strncpy(vm_in->name, mode_preset->name, 10);

    memcpy(vm_out, vm_in, sizeof(mode_data_t));
    vm_out->vic = HDMI_Unknown;

    memset(vm_conf, 0, sizeof(vm_proc_config_t));
    vm_conf->si_pclk_mult = 1;

    mindiff_lm &= mode_preset->flags;    //ensure L2 mode uniqueness

    // Set passthru flag for all passthru mode options
    if (*group_ptr[mode_preset->group] == 0)
        mindiff_lm = MODE_PT;

    if (mindiff_lm >= MODE_L6_GEN_4_3)
        vm_conf->y_rpt = 5;
    else if (mindiff_lm >= MODE_L5_GEN_4_3)
        vm_conf->y_rpt = 4;
    else if (mindiff_lm >= MODE_L4_GEN_4_3)
        vm_conf->y_rpt = 3;
    else if (mindiff_lm >= MODE_L3_GEN_16_9)
        vm_conf->y_rpt = 2;
    else if (mindiff_lm >= MODE_L2)
        vm_conf->y_rpt = 1;

    switch (mindiff_lm) {
        case MODE_PT:
            vm_out->vic = vm_in->vic;

            if ((cc->pt_mode == 1) && ((mode_preset->group >= GROUP_384P) && (mode_preset->group <= GROUP_576P))) {
                vmode_hv_mult(vm_in, 2, 1);
                vmode_hv_mult(vm_out, 2, 1);
            } else if ((cc->pt_mode >= 2) && (mode_preset->group >= GROUP_240P) && (mode_preset->group <= GROUP_288P)) {
                vm_conf->x_rpt = vm_conf->h_skip = cc->pt_mode;
            }

            // multiply horizontal resolution if necessary to fulfill min. 25MHz TMDS clock requirement. Tweak infoframe pixel repetition indicator later to make sink treat it as original resolution.
            while ((((vm_out->timings.v_hz_x100*vm_out->timings.v_total)/100)*vm_out->timings.h_total*(vm_conf->h_skip+1))>>vm_out->timings.interlaced < 25000000UL) {
                vm_conf->x_rpt = vm_conf->h_skip = 2*(vm_conf->h_skip+1)-1;
            }
            vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
            break;
        case MODE_L2:
            // Upsample / pixel-repeat horizontal resolution of 384p/480p/960i modes
            if ((mode_preset->group == GROUP_384P) || (mode_preset->group == GROUP_480P) || (mode_preset->group == GROUP_576P) || ((mode_preset->group == GROUP_1080I) && (mode_preset->timings.h_total < 1200))) {
                if (upsample2x) {
                    vmode_hv_mult(vm_in, 2, 1);
                    vmode_hv_mult(vm_out, 2, VM_OUT_YMULT);
                } else {
                    vm_conf->x_rpt = 1;
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                }
            } else {
                vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
            }
            break;
        case MODE_L3_GEN_16_9:
            // Upsample / pixel-repeat horizontal resolution of 480i mode
            if ((mode_preset->group == GROUP_480I) || (mode_preset->group == GROUP_576I)) {
                if (upsample2x) {
                    vmode_hv_mult(vm_in, 2, 1);
                    vmode_hv_mult(vm_out, 2, VM_OUT_YMULT);
                } else {
                    vm_conf->x_rpt = 1;
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                }
            } else {
                vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
            }
            break;
        case MODE_L3_GEN_4_3:
            vm_conf->x_size = vm_out->timings.h_active-2*vm_in->ar.h;
            vm_out->timings.h_synclen /= 3;
            vm_out->timings.h_backporch /= 3;
            vm_out->timings.h_active /= 3;
            vm_out->timings.h_total /= 3;
            vm_out->timings.h_total_adj = 0;
            vmode_hv_mult(vm_out, 4, VM_OUT_YMULT);
            break;
        case MODE_L4_GEN_4_3:
            // Upsample / pixel-repeat horizontal resolution of 480i mode
            if ((mode_preset->group == GROUP_480I) || (mode_preset->group == GROUP_576I)) {
                if (upsample2x) {
                    vmode_hv_mult(vm_in, 2, 1);
                    vmode_hv_mult(vm_out, 2, VM_OUT_YMULT);
                } else {
                    vm_conf->x_rpt = 1;
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                }
            } else {
                vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
            }
            break;
        case MODE_L5_GEN_4_3:
        case MODE_L6_GEN_4_3:
            vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
            break;
        case MODE_L2_512_COL:
        case MODE_L2_384_COL:
        case MODE_L2_320_COL:
        case MODE_L3_512_COL:
        case MODE_L4_512_COL:
        case MODE_L6_512_COL:
            vm_conf->x_rpt = vm_conf->h_skip = 1;
            vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
            break;
        case MODE_L2_256_COL:
        case MODE_L3_384_COL:
        case MODE_L4_384_COL:
        case MODE_L5_512_COL:
        case MODE_L6_384_COL:
        case MODE_L6_320_COL:
            vm_conf->x_rpt = vm_conf->h_skip = 2;
            vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
            break;
        case MODE_L3_240x360:
        case MODE_L3_320_COL:
        case MODE_L4_320_COL:
        case MODE_L5_384_COL:
        case MODE_L6_256_COL:
            vm_conf->x_rpt = vm_conf->h_skip = 3;
            vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
            break;
        case MODE_L2_240x360:
        case MODE_L3_256_COL:
        case MODE_L4_256_COL:
        case MODE_L5_320_COL:
            vm_conf->x_rpt = vm_conf->h_skip = 4;
            vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
            break;
        case MODE_L5_256_COL:
            vm_conf->x_rpt = vm_conf->h_skip = 5;
            vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
            break;
        default:
            printf("WARNING: invalid mindiff_lm\n");
            return -1;
            break;
    }

    // Set clock multiplication factor
    if (mindiff_lm == MODE_L3_GEN_4_3)
        vm_conf->si_pclk_mult = 4;
    else
        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;

    // Reduce x_rpt for 1:1 PAR 256col mode
    if (mindiff_lm & (MODE_L2_256_COL|MODE_L4_256_COL|MODE_L5_256_COL))
        vm_conf->x_rpt -= cc->ar_256col;
    else if (mindiff_lm & (MODE_L3_256_COL|MODE_L6_256_COL))
        vm_conf->x_rpt = cc->ar_256col ? 2 : 3;

    if (mindiff_lm & (MODE_L3_320_COL|MODE_L2_240x360|MODE_L3_240x360))
        vm_conf->x_rpt--;

    if (mindiff_lm == MODE_L2_240x360) {
        vm_out->timings.h_active += 80;
        vm_out->timings.h_backporch -= 40;
    }

    // Force TX pixel-repeat for high bandwidth modes
    if (((mindiff_lm == MODE_L5_GEN_4_3) && (mode_preset->group == GROUP_288P)) ||
       ((mindiff_lm == MODE_L3_GEN_16_9) && ((mode_preset->group == GROUP_480P) || (mode_preset->group == GROUP_576P))) ||
       (mindiff_lm == MODE_L3_240x360) || (mindiff_lm >= MODE_L6_GEN_4_3))
        vm_conf->tx_pixelrep = 1;

    sniprintf(vm_out->name, 11, "%s x%u", vm_in->name, vm_conf->y_rpt+1);

    if (vm_conf->x_size == 0)
        vm_conf->x_size = (vm_in->timings.h_active-2*vm_in->ar.h)*(vm_conf->x_rpt+1);
    if (vm_conf->y_size == 0)
        vm_conf->y_size = vm_out->timings.v_active-2*vm_in->ar.v*(vm_conf->y_rpt+1);

    vm_conf->x_offset = ((vm_out->timings.h_active-vm_conf->x_size)/2);
    vm_conf->x_start_lb = vm_in->ar.h;
    vm_conf->y_offset = ((vm_out->timings.v_active-vm_conf->y_size)/2);

    // Line5x format
    if ((vm_conf->y_rpt == 4) && !((mindiff_lm == MODE_L5_GEN_4_3) && (mode_preset->group == GROUP_288P))) {
        // adjust output width to 1920
        if (cc->l5_fmt != 1) {
            vm_conf->x_offset = (1920-vm_conf->x_size)/2;
            vm_out->timings.h_synclen = (vm_out->timings.h_total - 1920)/4;
            vm_out->timings.h_backporch = (vm_out->timings.h_total - 1920)/2;
            vm_out->timings.h_active = 1920;
        }

        // adjust output height to 1080
        if (cc->l5_fmt == 0) {
            vm_conf->y_start_lb = (vm_out->timings.v_active-1080)/10;
            vm_out->timings.v_backporch += 5*vm_conf->y_start_lb;
            vm_out->timings.v_active = 1080;
            vm_conf->y_size = vm_out->timings.v_active-2*vm_in->ar.v*(vm_conf->y_rpt+1);
        }
    }

    // Aspect
    if (vm_out->type & VIDEO_HDTV) {
        vm_out->ar.h = 16;
        vm_out->ar.v = 9;
    } else {
        vm_out->ar.h = 4;
        vm_out->ar.v = 3;
    }

#ifdef LM_EMIF_EXTRA_DELAY
    vm_conf->framesync_line = ((vm_out->timings.v_total>>vm_out->timings.interlaced)-(1+vm_out->timings.interlaced)*(vm_conf->y_rpt+1));
#else
    //vm_conf->framesync_line = vm_in->timings.interlaced ? ((vm_out->timings.v_total>>vm_out->timings.interlaced)-(vm_conf->y_rpt+1)) : 0;
    vm_conf->framesync_line = 0;
#endif
    vm_conf->framelock = 1;

    if (vm_out->vic == HDMI_Unknown)
        vm_out->vic = cc->default_vic;

    return mindiff_id;
}

int get_vmode(vmode_t vmode_id, mode_data_t *vm_in, mode_data_t *vm_out, vm_proc_config_t *vm_conf)
{
    memset(vm_conf, 0, sizeof(vm_proc_config_t));
    memset(vm_in, 0, sizeof(mode_data_t));
	memcpy(vm_out, &video_modes_plm_default[vmode_id], sizeof(mode_data_t));
	if (vm_out->type & VIDEO_HDTV)
	{
		vm_out->ar.h = 16;
		vm_out->ar.v = 9;
	}
	else
	{
		vm_out->ar.h = 4;
		vm_out->ar.v = 3;
	}

	return 0;
}
