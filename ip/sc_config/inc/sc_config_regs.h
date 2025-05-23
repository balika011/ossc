//
// Copyright (C) 2015-2019  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#ifndef SC_CONFIG_REGS_H_
#define SC_CONFIG_REGS_H_

#include <stdint.h>

// bit-fields coded as little-endian
typedef union {
    struct {
        uint16_t vtotal:11;
        uint8_t interlace_flag:1;
        uint8_t sync_active:1;
        uint32_t fe_rsv:19;
    } __attribute__((packed));
    uint32_t data;
} fe_status_reg;

typedef union {
    struct {
        uint32_t pcnt_field:20;
        uint8_t hsync_width:8;
        uint8_t fe_rsv:4;
    } __attribute__((packed));
    uint32_t data;
} fe_status2_reg;

typedef union {
    struct {
        uint16_t lt_lat_result:16;
        uint16_t lt_stb_result:12;
        uint8_t lt_rsv:3;
        uint8_t lt_finished:1;
    } __attribute__((packed));
    uint32_t data;
} lt_status_reg;

typedef union {
    struct {
		uint16_t ir_code;
		uint8_t ir_cnt;
		uint8_t btn1:1;
		uint8_t btn2:1;
		uint8_t hdmi_tx_mode:1;
		uint8_t is_1_8:1;
		uint8_t pll_activeclk:1;
		uint8_t vsync_flag:1;
		uint8_t resync_indicator:1;
		uint8_t reserved:1;
	} __attribute__((packed));
    uint32_t data;
} controls_reg;

typedef union {
    struct {
        uint16_t h_total:12;
        uint16_t h_active:12;
        uint16_t h_synclen:8;
    } __attribute__((packed));
    uint32_t data;
} hv_config_reg;

typedef union {
    struct {
        uint16_t h_backporch:9;
        uint16_t v_total:11;
        uint16_t v_active:11;
        uint8_t interlaced:1;
    } __attribute__((packed));
    uint32_t data;
} hv_config2_reg;

typedef union {
    struct {
        uint8_t v_synclen:4;
        uint16_t v_backporch:9;
        uint16_t v_startline:11;
        uint8_t h_skip:4;
        uint8_t h_sample_sel:4;
    } __attribute__((packed));
    uint32_t data;
} hv_config3_reg;

typedef union {
    struct {
        uint16_t x_size:12;
        uint16_t y_size:11;
        int16_t y_offset:9;
    } __attribute__((packed));
    uint32_t data;
} xy_config_reg;

typedef union {
    struct {
        int16_t x_offset:10;
        uint8_t x_start_lb:8;
        int8_t y_start_lb:6;
        int8_t x_rpt:4;
        int8_t y_rpt:4;
    } __attribute__((packed));
    uint32_t data;
} xy_config2_reg;

typedef union {
    struct {
        uint8_t mask_br:4;
        uint8_t mask_color:3;
        uint8_t reverse_lpf:5;
        uint8_t lm_deint_mode:1;
        uint8_t nir_even_offset:1;
        uint8_t ypbpr_cs:1;
        uint8_t vip_enable:1;
        uint8_t bfi_str:4;
        uint8_t bfi_enable:1;
        uint8_t shmask_mode:2;
        uint8_t lumacode_mode:3;
        uint32_t misc_rsv:6;
    } __attribute__((packed));
    uint32_t data;
} misc_config_reg;

typedef union {
    struct {
        uint32_t sl_l_str_arr:24;
        uint8_t sl_l_overlay:6;
        uint8_t sl_method:1;
        uint8_t sl_altern:1;
    } __attribute__((packed));
    uint32_t data;
} sl_config_reg;

typedef union {
    struct {
        uint32_t sl_c_str_arr_l;
    } __attribute__((packed));
    uint32_t data;
} sl_config2_reg;

typedef union {
    struct {
        uint32_t sl_c_str_arr_h:8;
        uint32_t sl_c_overlay:10;
        uint8_t sl_iv_y:3;
        uint8_t sl_iv_x:4;
        uint8_t sl_hybr_str:5;
        uint32_t sl_rsv:2;
    } __attribute__((packed));
    uint32_t data;
} sl_config3_reg;

typedef union {
    struct {
		uint8_t hw_reset:1;
		uint8_t av_reset_n:1;
		uint8_t enable_sc:1;
		uint8_t lcd_bl_on:1;
		uint8_t lcd_rs:1;
		uint8_t lcd_cs_n:1;
		uint8_t vsync_type:1;
		uint8_t led_g:1;
		uint8_t led_r:1;
		uint8_t reserved_1_2:1;
		uint8_t reserved_1_3:1;
		uint8_t reserved_1_4:1;
		uint8_t reserved_1_5:1;
		uint8_t reserved_1_6:1;
		uint8_t reserved_1_7:1;
		uint8_t reserved_1_8:1;
		uint8_t reserved_2_1:1;
		uint8_t reserved_2_2:1;
		uint8_t reserved_2_3:1;
		uint8_t reserved_2_4:1;
		uint8_t reserved_2_5:1;
		uint8_t reserved_2_6:1;
		uint8_t reserved_2_7:1;
		uint8_t reserved_2_8:1;
		uint8_t reserved_3_1:1;
		uint8_t reserved_3_2:1;
		uint8_t reserved_3_3:1;
		uint8_t reserved_3_4:1;
		uint8_t reserved_3_5:1;
		uint8_t reserved_3_6:1;
		uint8_t remap_red_r:1;
		uint8_t remap_lcd_bl:1;
	} __attribute__((packed));
    uint32_t data;
} sys_ctrl_reg;

typedef struct {
    fe_status_reg fe_status;
    fe_status2_reg fe_status2;
    lt_status_reg lt_status;
	controls_reg controls;

	hv_config_reg hv_in_config;
	hv_config2_reg hv_in_config2;
    hv_config3_reg hv_in_config3;
    hv_config_reg hv_out_config;
    hv_config2_reg hv_out_config2;
    hv_config3_reg hv_out_config3;
    xy_config_reg xy_out_config;
    xy_config2_reg xy_out_config2;
    misc_config_reg misc_config;
    sl_config_reg sl_config;
    sl_config2_reg sl_config2;
    sl_config3_reg sl_config3;
	sys_ctrl_reg sys_ctrl;
} sc_regs;

#endif //SC_CONFIG_REGS_H_
