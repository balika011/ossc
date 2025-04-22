//
// Copyright (C) 2025  Balázs Triszka <info@balika011.hu>
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

#include <stdint.h>
#include "system.h"
#include "timer.h"

#include "altera_avalon_timer_regs.h"

static uint64_t timer_ctr;
static uint64_t timeout_ctr[5];
static void (*timeout_cb[5])();

// called every 100us
void __attribute__((interrupt, noinline, __section__(".rtext"))) timer_irq_handler(void)
{
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0);
	IORD_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE);

	timer_ctr += 100;

	for (int i = 0; i < 5; i++)
	{
		if (timeout_ctr[i])
		{
			if (timeout_ctr[i] < timer_ctr)
			{
				timeout_ctr[i] = 0;
				timeout_cb[i]();
			}
		}
	}
}

void timer_init()
{
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, ALTERA_AVALON_TIMER_CONTROL_ITO_MSK | ALTERA_AVALON_TIMER_CONTROL_CONT_MSK | ALTERA_AVALON_TIMER_CONTROL_START_MSK);
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0);
	IORD_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE);
	asm volatile("csrs  mie, %0\n" : : "r"(0x80));
	asm volatile("csrs  mstatus, %0\n" : : "r"(0x8));
}

void timer_deinit()
{
	asm volatile("csrc  mie, %0\n" : : "r"(0x80));
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, ALTERA_AVALON_TIMER_CONTROL_STOP_MSK);
}

uint64_t __attribute__((noinline, __section__(".rtext"))) timer_timestamp()
{
	return timer_ctr;
}

int timer_timeout(uint64_t usec, void (*cb)())
{
	int idx = -1;
	for (int i = 0; i < 5; i++)
	{
		if (!timeout_ctr[i])
		{
			idx = i;
			break;
		}
	}

	if (idx >= 0)
	{
		timeout_ctr[idx] = timer_ctr + usec;
		timeout_cb[idx] = cb;
	}

	return idx;
}

void timer_cancel(int idx)
{
	if (idx >= 0)
		timeout_ctr[idx] = 0;
}