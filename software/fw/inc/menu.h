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

#ifndef MENU_H_
#define MENU_H_

#include <stdint.h>
#include "sysconfig.h"
#include "controls.h"
#include "video_modes.h"

#ifdef OSDLANG_JP
#define LNG(e, j) j
#else
#define LNG(e, j) e
#endif

typedef enum
{
	OPT_AVCONFIG_SELECTION,
	OPT_AVCONFIG_NUMVALUE,
	OPT_AVCONFIG_NUMVAL_U16,
	OPT_SUBMENU,
	OPT_FUNC_CALL,
} menuitem_type;

typedef int (*func_call)(void);
typedef void (*arg_func)(void);
typedef void (*disp_func)(uint8_t);
typedef void (*disp_func_u16)(uint16_t *);

typedef struct
{
	uint8_t *data;
	uint8_t max;
	disp_func df;
} arg_info_t;

typedef struct
{
	uint8_t *data;
	uint8_t wrap_cfg;
	uint8_t min;
	uint8_t max;
	const char **setting_str;
} opt_avconfig_selection;

typedef struct
{
	uint8_t *data;
	uint8_t wrap_cfg;
	uint8_t min;
	uint8_t max;
	disp_func df;
} opt_avconfig_numvalue;

typedef struct
{
	uint16_t *data;
	uint16_t min;
	uint16_t max;
	disp_func_u16 df;
} opt_avconfig_numvalue_u16;

typedef struct
{
	func_call f;
	const arg_info_t *arg_info;
} opt_func_call;

typedef struct menustruct menu_t;

typedef struct
{
	const menu_t *menu;
	const arg_info_t *arg_info;
	arg_func arg_f;
} opt_submenu;

typedef struct
{
	const char *name;
	menuitem_type type;
	union
	{
		opt_avconfig_selection sel;
		opt_avconfig_numvalue num;
		opt_avconfig_numvalue_u16 num_u16;
		opt_submenu sub;
		opt_func_call fun;
	};
} menuitem_t;

struct menustruct
{
	uint8_t num_items;
	menuitem_t *items;
};

typedef struct
{
	const menu_t *m;
	uint8_t mp;
} menunavi;

extern uint16_t tc_sampler_phase;
extern uint8_t vm_sel, vm_edit;
extern uint8_t menu_active;

menunavi* get_current_menunavi();
void init_menu();
void menu_render_page();
void display_menu(uint8_t forcedisp);
static void vm_select();
static void vm_tweak(uint16_t *v);

void menu_sampler_phase(uint8_t v);
void menu_scanlines_mode();
void menu_scanlines_type();
void menu_scanlines_strength();

#endif
