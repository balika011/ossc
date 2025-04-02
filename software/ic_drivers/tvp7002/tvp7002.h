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

#ifndef TVP7002_H_
#define TVP7002_H_

#include "tvp7002_regs.h"
#include "video_modes.h"
#include "sysconfig.h"

#define DEFAULT_VSYNC_THOLD     0x44
#define DEFAULT_LINELEN_TOL     0x06
#define DEFAULT_PRE_COAST       1
#define DEFAULT_POST_COAST      0
#define DEFAULT_SYNC_LPF        0
#define DEFAULT_SYNC_VTH        0x0B
#define DEFAULT_FINE_GAIN       26
#define DEFAULT_FINE_OFFSET     0x80
#define DEFAULT_COARSE_GAIN     0x8
#define DEFAULT_ALC_H_FILTER    0x3
#define DEFAULT_ALC_V_FILTER    0x9

#define TVP_INTCLK_HZ           6500000UL
#define TVP_EXTCLK_HZ           27000000UL

typedef enum {
    TVP_INPUT1 = 0,
    TVP_INPUT2 = 1,
    TVP_INPUT3 = 2
} tvp_input_t;

typedef enum {
    TVP_SOG1 = 0,
    TVP_SOG2 = 1,
    TVP_SOG3 = 2,
    TVP_HV_A = 3,
    TVP_HV_B = 4,
    TVP_CS_A = 5,
    TVP_CS_B = 6
} tvp_sync_input_t;

typedef enum {
    REFCLK_EXT27    = 0,
    REFCLK_INTCLK   = 1
} tvp_refclk_t;

typedef struct {
    const char *name;
    uint16_t R_Y;
    uint16_t R_Pb;
    uint16_t R_Pr;
    uint16_t G_Y;
    uint16_t G_Pb;
    uint16_t G_Pr;
    uint16_t B_Y;
    uint16_t B_Pb;
    uint16_t B_Pr;
} ypbpr_to_rgb_csc_t;

extern const ypbpr_to_rgb_csc_t csc_coeffs[];

inline uint32_t tvp_readreg(uint32_t regaddr);

inline void tvp_writereg(uint32_t regaddr, uint8_t data);

inline void tvp_reset();

inline void tvp_disable_output();

inline void tvp_enable_output();

inline void tvp_powerdown();

inline void tvp_powerup();

inline void tvp_set_hpllcoast(uint8_t pre, uint8_t post);

inline void tvp_set_linelen_tol(uint8_t val);

inline void tvp_set_ssthold(uint8_t vsdetect_thold);

void tvp_init();

void tvp_set_gain_offset(color_setup_t *col);

void tvp_setup_hpll(uint16_t h_samplerate, uint16_t pixs_per_line, uint16_t refclks_per_line, uint8_t plldivby2);

void tvp_sel_csc(const ypbpr_to_rgb_csc_t *csc);

void tvp_set_lpf(uint8_t val);

void tvp_set_sync_lpf(uint8_t val);

void tvp_set_clp_lpf(uint8_t val);

void tvp_set_hpll_phase(uint8_t val);

void tvp_set_sog_thold(uint8_t val);

void tvp_set_alcfilt(uint8_t nsv, uint8_t nsh);

void tvp_source_setup(video_type type, uint16_t h_samplerate, uint16_t pixs_per_line, uint16_t refclks_per_line, uint8_t plldivby2, uint8_t h_synclen_px, int8_t clamp_user_offset, uint8_t vmode_changed);

void tvp_source_sel(tvp_input_t input, tvp_sync_input_t syncinput, video_format fmt);

uint8_t tvp_check_sync(tvp_sync_input_t syncinput);

#endif /* TVP7002_H_ */
