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

.section .text

copy_loop:
	lbu a5, 0(a0)
	addi a0, a0, 1
	addi a1, a1, 1
	sb a5, -1(a1)
copy:
	bne a1, a2, copy_loop
	ret

reset_handler:
	/* set all registers to zero */
	mv ra, x0
	mv sp, ra
	mv gp, ra
	mv tp, ra
	mv t0, ra
	mv t1, ra
	mv t2, ra
	mv s0, ra
	mv s1, ra
	mv a0, ra
	mv a1, ra
	mv a2, ra
	mv a3, ra
	mv a4, ra
	mv a5, ra

	/* stack initilization */
	la	 sp, _stack_start

_start:
	.global _start

	/* clear BSS */
	la a4, _bss_start
	la a5, _bss_end

	bge a4, a5, zero_loop_end

zero_loop:
	sw x0, 0(a4)
	addi a4, a4, 4
	ble a4, a5, zero_loop
zero_loop_end:

	la a2, _ervectors
	la a1, _srvectors
	la a0, _frvectors
	jal ra, copy

	la a2, _ertext
	la a1, _srtext
	la a0, _frtext
	jal ra, copy

	la a2, _edata
	la a1, _sdata
	la a0, _fdata
	jal ra, copy

	la a0, mtvec_handler
	csrw mtvec, a0

main_entry:
	jal ra, main

	/* If execution ends up here just put the core to sleep */
sleep_loop:
	wfi
	j sleep_loop

/* =================================================== [ exceptions ] === */
/* This section has to be down here, since we have to disable rvc for it	*/

	.section .vectors, "ax"
	.option norvc;

	// All unimplemented interrupts/exceptions go to the default_exc_handler.
	.org 0x00
	.rept 32
	j .
	.endr
	jal x0, reset_handler
