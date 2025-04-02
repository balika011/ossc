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

#ifndef USERDATA_H_
#define USERDATA_H_

#include <stdint.h>
#include "sysconfig.h"
#include "controls.h"
#include "av_controller.h"
#include "avconfig.h"
#include "video_modes.h"
#include "flash.h"

#define PROFILE_NAME_LEN 12

#define MAX_USERDATA_ENTRY 15
#define MAX_PROFILE (MAX_USERDATA_ENTRY-1)
#define INIT_CONFIG_SLOT MAX_USERDATA_ENTRY

#define UDATA_IMPT_CANCELLED 104
#define UDATA_EXPT_CANCELLED 105

typedef enum {
    UDE_INITCFG  = 0,
    UDE_PROFILE,
} ude_type;

typedef struct {
    char magic[8];
    uint8_t version_major;
    uint8_t version_minor;
    ude_type type;
} __attribute__((packed, __may_alias__)) ude_hdr;

typedef struct {
    ude_hdr hdr;
    uint16_t data_len;
    uint8_t last_profile[AV_LAST];
    uint8_t profile_link;
    avinput_t last_input;
    avinput_t def_input;
    uint8_t lcd_bl_timeout;
    uint8_t auto_input;
    uint8_t auto_av1_ypbpr;
    uint8_t auto_av2_ypbpr;
    uint8_t auto_av3_ypbpr;
    uint8_t osd_enable;
    uint8_t osd_status_timeout;
    uint8_t phase_hotkey_enable;
    uint16_t keys[REMOTE_MAX_KEYS];
} __attribute__((packed, __may_alias__)) ude_initcfg;

typedef struct {
    ude_hdr hdr;
    char name[PROFILE_NAME_LEN+1];
    uint16_t avc_data_len;
    uint16_t vm_data_len;
    avconfig_t avc;
    //mode_data_t vm[VIDEO_MODES_CNT];
} __attribute__((packed, __may_alias__)) ude_profile;

void userdata_migrate();

int userdata_save(uint8_t entry);
int userdata_load(uint8_t entry, int dry_run);
int userdata_import();
int userdata_export();

#endif
