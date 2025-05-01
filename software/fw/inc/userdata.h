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

extern char target_profile_name[PROFILE_NAME_LEN + 1];

int userdata_save_initconfig();
int userdata_save_profile(uint8_t entry);
int userdata_load_initconfig();
int userdata_load_profile(uint8_t entry, int dry_run);
int userdata_import();
int userdata_export();

#endif
