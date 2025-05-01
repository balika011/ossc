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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.	If not, see <http://www.gnu.org/licenses/>.
//

.section .rtext

.weak default_exc_handler
.global default_exc_handler
default_exc_handler:
	j .

.weak software_irq_handler
.global software_irq_handler
software_irq_handler:
	j .

.weak timer_irq_handler
.global timer_irq_handler
timer_irq_handler:
	j .

.weak external_irq_handler
.global external_irq_handler
external_irq_handler:
	j .

.weak fast_irq_0_handler
.global fast_irq_0_handler
fast_irq_0_handler:
	j .

.weak fast_irq_1_handler
.global fast_irq_1_handler
fast_irq_1_handler:
	j .

.weak fast_irq_2_handler
.global fast_irq_2_handler
fast_irq_2_handler:
	j .

.weak fast_irq_3_handler
.global fast_irq_3_handler
fast_irq_3_handler:
	j .

.weak fast_irq_4_handler
.global fast_irq_4_handler
fast_irq_4_handler:
	j .

.weak fast_irq_5_handler
.global fast_irq_5_handler
fast_irq_5_handler:
	j .

.weak fast_irq_6_handler
.global fast_irq_6_handler
fast_irq_6_handler:
	j .

.weak fast_irq_7_handler
.global fast_irq_7_handler
fast_irq_7_handler:
	j .

.weak fast_irq_8_handler
.global fast_irq_8_handler
fast_irq_8_handler:
	j .

.weak fast_irq_9_handler
.global fast_irq_9_handler
fast_irq_9_handler:
	j .

.weak fast_irq_10_handler
.global fast_irq_10_handler
fast_irq_10_handler:
	j .

.weak fast_irq_11_handler
.global fast_irq_11_handler
fast_irq_11_handler:
	j .

.weak fast_irq_12_handler
.global fast_irq_12_handler
fast_irq_12_handler:
	j .

.weak fast_irq_13_handler
.global fast_irq_13_handler
fast_irq_13_handler:
	j .

.weak fast_irq_14_handler
.global fast_irq_14_handler
fast_irq_14_handler:
	j .

.weak nmi_irq_handler
.global nmi_irq_handler
nmi_irq_handler:
	j .

/* =================================================== [ exceptions ] === */
/* This section has to be down here, since we have to disable rvc for it	*/

	.section .rvectors, "ax"
	.option norvc;

.global mtvec_handler
mtvec_handler:
	// All unimplemented interrupts/exceptions go to the default_exc_handler.
	.org 0x00
	jal x0, default_exc_handler // 0
	jal x0, default_exc_handler // 1
	jal x0, default_exc_handler // 2
	jal x0, software_irq_handler // 3 - irq_software_i
	jal x0, default_exc_handler // 4
	jal x0, default_exc_handler // 5
	jal x0, default_exc_handler // 6
	jal x0, timer_irq_handler // 7 - irq_timer_i
	jal x0, default_exc_handler // 8
	jal x0, default_exc_handler // 9
	jal x0, default_exc_handler // 10
	jal x0, external_irq_handler // 11 - irq_external_i
	jal x0, default_exc_handler // 12
	jal x0, default_exc_handler // 13
	jal x0, default_exc_handler // 14
	jal x0, default_exc_handler // 15
	jal x0, fast_irq_0_handler // 16 - irq_fast_i[0]
	jal x0, fast_irq_1_handler // 17 - irq_fast_i[1]
	jal x0, fast_irq_2_handler // 18 - irq_fast_i[2]
	jal x0, fast_irq_3_handler // 19 - irq_fast_i[3]
	jal x0, fast_irq_4_handler // 20 - irq_fast_i[4]
	jal x0, fast_irq_5_handler // 21 - irq_fast_i[5]
	jal x0, fast_irq_6_handler // 22 - irq_fast_i[6]
	jal x0, fast_irq_7_handler // 23 - irq_fast_i[7]
	jal x0, fast_irq_8_handler // 24 - irq_fast_i[8]
	jal x0, fast_irq_9_handler // 25 - irq_fast_i[9]
	jal x0, fast_irq_10_handler // 26 - irq_fast_i[10]
	jal x0, fast_irq_11_handler // 27 - irq_fast_i[11]
	jal x0, fast_irq_12_handler // 28 - irq_fast_i[12]
	jal x0, fast_irq_13_handler // 29 - irq_fast_i[13]
	jal x0, fast_irq_14_handler // 30 - irq_fast_i[14]
	jal x0, nmi_irq_handler // 31 - irq_nm_i
	j . // 32 - reset vector
