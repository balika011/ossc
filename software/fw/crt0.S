# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

#define EXCEPTION_STACK_SIZE 72

.section .text

default_exc_handler:
software_irq_handler:
external_irq_handler:
nmi_irq_handler:
fast_irq_0_handler:
fast_irq_1_handler:
fast_irq_2_handler:
fast_irq_3_handler:
fast_irq_4_handler:
fast_irq_5_handler:
fast_irq_6_handler:
fast_irq_7_handler:
fast_irq_8_handler:
fast_irq_9_handler:
fast_irq_10_handler:
fast_irq_11_handler:
fast_irq_12_handler:
fast_irq_13_handler:
fast_irq_14_handler:
  jal x0, ossc_exc_handler

timer_irq_handler:
  jal x0, ossc_timer_handler

reset_handler:
  /* set all registers to zero */
  mv  x1, x0
  mv  x2, x1
  mv  x3, x1
  mv  x4, x1
  mv  x5, x1
  mv  x6, x1
  mv  x7, x1
  mv  x8, x1
  mv  x9, x1
  mv x10, x1
  mv x11, x1
  mv x12, x1
  mv x13, x1
  mv x14, x1
  mv x15, x1

  /* stack initilization */
  la   x2, _stack_start

_start:
  .global _start

  /* clear BSS */
  la x14, _bss_start
  la x15, _bss_end

  bge x14, x15, zero_loop_end

zero_loop:
  sw x0, 0(x14)
  addi x14, x14, 4
  ble x14, x15, zero_loop
zero_loop_end:

  jal init_ram

  la   x10, mtvec_handler
  csrw mtvec, x10

main_entry:
  /* jump to main program entry point (argc = argv = 0) */
  addi x10, x0, 0
  addi x11, x0, 0
  jal x1, main

  /* If execution ends up here just put the core to sleep */
sleep_loop:
  wfi
  j sleep_loop

/* =================================================== [ exceptions ] === */
/* This section has to be down here, since we have to disable rvc for it  */

  .section .vectors, "ax"
  .option norvc;

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
  jal x0, reset_handler // 32 - reset vector
