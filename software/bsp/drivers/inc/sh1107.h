//
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

#ifndef SH1107_H_
#define SH1107_H_

#include "system.h"
#include <stdio.h>

int sh1107_init();

void sh1107_on();
void sh1107_off();

void sh1107_write_row1(const char *str);
void sh1107_write_row2(const char *str);

#endif /* sh1107_H_ */
