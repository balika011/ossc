//
// Copyright (C) 2015-2018  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#ifndef CONTROLS_H_
#define CONTROLS_H_

#include "sysconfig.h"

typedef enum
{
	KEY_NONE = -1,
	RC_BTN1,
	RC_BTN2,
	RC_BTN3,
	RC_BTN4,
	RC_BTN5,
	RC_BTN6,
	RC_BTN7,
	RC_BTN8,
	RC_BTN9,
	RC_BTN0,
	RC_MENU,
	RC_OK,
	RC_BACK,
	RC_UP,
	RC_DOWN,
	RC_LEFT,
	RC_RIGHT,
	RC_INFO,
	RC_STANDBY,
	RC_SL_MODE,
	RC_SL_TYPE,
	RC_SL_PLUS,
	RC_SL_MINUS,
	RC_LM_MODE,
	RC_PHASE_PLUS,
	RC_PHASE_MINUS,
	RC_PROF_HOTKEY,
	REMOTE_MAX_KEYS,
	FP_BTN1,
	FP_BTN2
} key_code_t;

extern uint32_t remote_code;
extern uint16_t rc_keymap[REMOTE_MAX_KEYS];
extern uint32_t btn1, btn2;

void controls_set_default();
void controls_setup();
int controls_parse();
void controls_update();

#endif
