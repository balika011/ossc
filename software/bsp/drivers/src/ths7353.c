//
// Copyright (C) 2015-2016  Markus Hiienkari <mhiienka@niksula.hut.fi>
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
#include "system.h"
#include "i2c_opencores.h"
#include "ths7353.h"

static uint32_t ths_readreg(uint8_t channel)
{
	// Phase 1
	I2C_start(I2CA_BASE, THS_BASE, 0);
	I2C_write(I2CA_BASE, channel, 1);

	// Phase 2
	I2C_start(I2CA_BASE, THS_BASE, 1);
	return I2C_read(I2CA_BASE, 1);
}

static void ths_writereg(uint8_t channel, uint8_t data)
{
	I2C_start(I2CA_BASE, THS_BASE, 0);
	I2C_write(I2CA_BASE, channel, 0);
	I2C_write(I2CA_BASE, data, 1);
}

int ths_init()
{
	// Avoid random FIFO state (see datasheet p.37)
	I2C_write(I2CA_BASE, 0x00, 0);
	usleep(10);

	// Initialize all channels
	ths_writereg(THS_CH1, (THS_LPF_DEFAULT << THS_LPF_OFFS));
	ths_writereg(THS_CH2, (THS_LPF_DEFAULT << THS_LPF_OFFS));
	ths_writereg(THS_CH3, (THS_LPF_DEFAULT << THS_LPF_OFFS));

	return (ths_readreg(THS_CH1) == (THS_LPF_DEFAULT << THS_LPF_OFFS));
}

void ths_set_lpf(uint8_t val)
{
	uint8_t status = ths_readreg(THS_CH1) & ~THS_LPF_MASK;
	status |= (val << THS_LPF_OFFS);

	ths_writereg(THS_CH1, status);
	ths_writereg(THS_CH2, status);
	ths_writereg(THS_CH3, status);
	printf("THS LPF value set to 0x%x\n", val);
}

void ths_source_sel(ths_input_t input, uint8_t lpf)
{
	uint8_t status = ths_readreg(THS_CH1) & ~(THS_SRC_MASK | THS_MODE_MASK);

	if (input == THS_STANDBY)
		status |= (THS_MODE_AVMUTE << THS_MODE_OFFS);
	else
		status |= (THS_MODE_AC_BIAS | (input << THS_SRC_OFFS));

	ths_writereg(THS_CH1, status);
	ths_writereg(THS_CH2, status);
	ths_writereg(THS_CH3, status);
	printf("THS source set to %u\n", input);
}
