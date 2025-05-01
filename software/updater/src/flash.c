//
// Copyright (C) 2015-2016  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#include <unistd.h>
#include <string.h>
#include "system.h"
#include "flash.h"
#include "utils.h"

typedef union {
	struct {
		uint32_t enable : 1;
		uint32_t reserved : 3;
		uint32_t chip_select : 4;
		uint32_t addressing_mode : 1;
	} __attribute__((packed));
	uint32_t data;
} flash_ctrl_t;

typedef union {
	struct {
		uint32_t divisor : 4;
	} __attribute__((packed));
	uint32_t data;
} flash_baud_rate_t;

typedef union {
	struct {
		uint32_t cs_assert : 4;
		uint32_t cs_deassert : 4;
		uint32_t cs_hold : 4; // tSHSL
	} __attribute__((packed));
	uint32_t data;
} flash_cs_delay_t;

typedef union {
	struct {
		uint32_t read_delay : 4;
	} __attribute__((packed));
	uint32_t data;
} flash_read_capture_t;

typedef union {
	struct {
		uint32_t cmd_transfer_mode : 2; // 0 standard, 1 dual, 2 quad
		uint32_t reserved_0 : 2;
		uint32_t write_address_transfer_mode : 2; // 0 standard, 1 dual, 2 quad
		uint32_t reserved_1 : 2;
		uint32_t write_data_transfer_mode : 2; // 0 standard, 1 dual, 2 quad
		uint32_t reserved_3 : 2;
		uint32_t read_address_transfer_mode : 2; // 0 standard, 1 dual, 2 quad
		uint32_t reserved_4 : 2;
		uint32_t read_data_transfer_mode : 2; // 0 standard, 1 dual, 2 quad
	} __attribute__((packed));
	uint32_t data;
} flash_oper_mode_t;

typedef union {
	struct {
		uint32_t opcode : 8;
		uint32_t dummy_cycles : 5;
	} __attribute__((packed));
	uint32_t data;
} flash_read_instr_t;

typedef union {
	struct {
		uint32_t write_opcode : 8;
		uint32_t poll_opcode : 8;
	} __attribute__((packed));
	uint32_t data;
} flash_write_instr_t;

typedef union {
	struct {
		uint32_t opcode : 8;
		uint32_t address_bytes : 3; // 3 or 4
		uint32_t data_type : 1; // 0 write, 1 read
		uint32_t data_bytes : 4;
		uint32_t dummy_cycles : 5;
	} __attribute__((packed));
	uint32_t data;
} flash_flash_cmd_cfg_t;

typedef union {
	struct {
		uint32_t start : 1;
	} __attribute__((packed));
	uint32_t data;
} flash_flash_cmd_ctrl_t;

typedef struct {
	volatile uint32_t ctrl;				// flash_ctrl_t
	volatile uint32_t baud_rate;		// flash_baud_rate_t
	volatile uint32_t cs_delay;			// flash_cs_delay_t
	volatile uint32_t read_capture;		// flash_read_capture_t
	volatile uint32_t oper_mode;		// flash_oper_mode_t
	volatile uint32_t read_instr;		// flash_read_instr_t
	volatile uint32_t write_instr;		// flash_write_instr_t
	volatile uint32_t flash_cmd_cfg;	// flash_flash_cmd_cfg_t
	volatile uint32_t flash_cmd_ctrl;	// flash_flash_cmd_ctrl_t
	volatile uint32_t flash_cmd_addr;
	volatile uint32_t flash_cmd_wrdata[2];
	volatile uint32_t flash_cmd_rddata[2];
} gen_flash_if_regs;

gen_flash_if_regs *flash_csr = (gen_flash_if_regs *) FLASH_CSR_BASE;

#define CMD_WRITE_STATUS_REGISTER 0x01
#define CMD_PAGE_WRITE 0x02
#define CMD_READ_DATA 0x03
#define CMD_WRITE_DISABLE 0x04
#define CMD_READ_STATUS_REGISTER 0x05
#define CMD_WRITE_ENABLE 0x06
#define CMD_FAST_READ 0x0B
#define CMD_ERASE_4K 0x20
#define CMD_QUAD_PAGE_WRITE 0x32
#define CMD_READ_STATUS_REGISTER_2 0x35
#define CMD_DUAL_FAST_READ_OUTPUT 0x3B
#define CMD_WRITE_SECURITY_REGISTERS 0x42
#define CMD_ERASE_SECURITY_REGISTERS 0x44
#define CMD_READ_SECURITY_REGISTERS 0x48
#define CMD_UNIQUE_ID 0x4B
#define CMD_WRITE_ENABLE_FOR_VOLATILE_STATUS_REGISTER 0x50
#define CMD_ERASE_32K 0x52
#define CMD_SFPD_REGISTER 0x5A
#define CMD_CHIP_ERASE_2 0x60
#define CMD_ENABLE_RESET 0x66
#define CMD_QUAD_FAST_READ_OUTPUT 0x6B
#define CMD_ERASE_WRITE_SUSPED 0x75
#define CMD_SET_BURST_WITH_WRAP 0x77
#define CMD_ERASE_WRITE_RESUME 0x7A
#define CMD_MANUFACTURER_ID 0x90
#define CMD_DUAL_MANUFACTURER_ID 0x92
#define CMD_QUAD_MANUFACTURER_ID 0x94
#define CMD_RESET 0x99
#define CMD_JEDEC_ID 0x9F
#define CMD_POWER_UP_DEVICE_ID 0xAB
#define CMD_CHIP_ERASE 0xC7
#define CMD_POWER_DOWN 0xB9
#define CMD_DUAL_FAST_READ_INOUT_OUTPUT 0xBB
#define CMD_ERASE_64K 0xD8
#define CMD_OCTAL_QUAD_READ_INOUT_OUTPUT 0xE3
#define CMD_WORD_QUAD_READ_INOUT_OUTPUT 0xE7
#define CMD_QUAD_FAST_READ_INOUT_OUTPUT 0xEB
#define CMD_CONTINOUS_READ_MODE 0xFF

void flash_write_protect(int enable)
{
	// Write enable
	flash_flash_cmd_cfg_t flash_cmd_cfg;
	flash_cmd_cfg.opcode = CMD_WRITE_ENABLE;
	flash_cmd_cfg.address_bytes = 0;
	flash_cmd_cfg.data_type = 0;
	flash_cmd_cfg.data_bytes = 0;
	flash_cmd_cfg.dummy_cycles = 0;
	flash_csr->flash_cmd_cfg = flash_cmd_cfg.data;
	flash_csr->flash_cmd_ctrl = 1;

	// Write status register
	flash_cmd_cfg.opcode = CMD_WRITE_STATUS_REGISTER;
	flash_cmd_cfg.address_bytes = 0;
	flash_cmd_cfg.data_type = 0;
	flash_cmd_cfg.data_bytes = 1;
	flash_cmd_cfg.dummy_cycles = 0;
	flash_csr->flash_cmd_cfg = flash_cmd_cfg.data;
	flash_csr->flash_cmd_wrdata[0] = enable ? 0x0000005c : 0x00000000;
	flash_csr->flash_cmd_ctrl = 1;

	// Poll status register until write has completed
	while (1)
	{
		flash_cmd_cfg.opcode = CMD_READ_STATUS_REGISTER;
		flash_cmd_cfg.address_bytes = 0;
		flash_cmd_cfg.data_type = 1;
		flash_cmd_cfg.data_bytes = 1;
		flash_cmd_cfg.dummy_cycles = 0;
		flash_csr->flash_cmd_cfg = flash_cmd_cfg.data;
		flash_csr->flash_cmd_ctrl = 1;
		if (!(flash_csr->flash_cmd_rddata[0] & (1 << 0)))
			break;
	}

	// Write disable
	flash_cmd_cfg.opcode = CMD_WRITE_DISABLE;
	flash_cmd_cfg.address_bytes = 0;
	flash_cmd_cfg.data_type = 0;
	flash_cmd_cfg.data_bytes = 0;
	flash_cmd_cfg.dummy_cycles = 0;
	flash_csr->flash_cmd_cfg = flash_cmd_cfg.data;
	flash_csr->flash_cmd_ctrl = 1;
}

void flash_erase_64k(uint32_t addr)
{
	// Write enable
	flash_flash_cmd_cfg_t flash_cmd_cfg;
	flash_cmd_cfg.opcode = CMD_WRITE_ENABLE;
	flash_cmd_cfg.address_bytes = 0;
	flash_cmd_cfg.data_type = 0;
	flash_cmd_cfg.data_bytes = 0;
	flash_cmd_cfg.dummy_cycles = 0;
	flash_csr->flash_cmd_cfg = flash_cmd_cfg.data;
	flash_csr->flash_cmd_ctrl = 1;

	// Sector erase
	flash_cmd_cfg.opcode = CMD_ERASE_64K;
	flash_cmd_cfg.address_bytes = 3;
	flash_cmd_cfg.data_type = 0;
	flash_cmd_cfg.data_bytes = 0;
	flash_cmd_cfg.dummy_cycles = 0;
	flash_csr->flash_cmd_cfg = flash_cmd_cfg.data;
	flash_csr->flash_cmd_addr = addr;
	flash_csr->flash_cmd_ctrl = 1;

	// Poll status register until write has completed
	while (1)
	{
		flash_cmd_cfg.opcode = CMD_READ_STATUS_REGISTER;
		flash_cmd_cfg.address_bytes = 0;
		flash_cmd_cfg.data_type = 1;
		flash_cmd_cfg.data_bytes = 1;
		flash_cmd_cfg.dummy_cycles = 0;
		flash_csr->flash_cmd_cfg = flash_cmd_cfg.data;
		flash_csr->flash_cmd_ctrl = 1;
		if (!(flash_csr->flash_cmd_rddata[0] & (1 << 0)))
			break;
	}
}