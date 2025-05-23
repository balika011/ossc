#ifndef __SYSTEM_H_
#define __SYSTEM_H_

/* Include definitions from linker script generator */
#include "sc_config_regs.h"
#include "osd_generator_regs.h"
#include "pll_reconfig_regs.h"


/*
 * CPU configuration
 *
 */

#define ALT_CPU_ARCHITECTURE "altera_nios2_gen2"
#define ALT_CPU_BIG_ENDIAN 0
#define ALT_CPU_BREAK_ADDR 0x00000820
#define ALT_CPU_CPU_ARCH_NIOS2_R1
#define ALT_CPU_CPU_FREQ 54000000u
#define ALT_CPU_CPU_ID_SIZE 1
#define ALT_CPU_CPU_ID_VALUE 0x00000000
#define ALT_CPU_CPU_IMPLEMENTATION "tiny"
#define ALT_CPU_DATA_ADDR_WIDTH 0x18
#define ALT_CPU_DCACHE_LINE_SIZE 0
#define ALT_CPU_DCACHE_LINE_SIZE_LOG2 0
#define ALT_CPU_DCACHE_SIZE 0
#define ALT_CPU_EXCEPTION_ADDR 0x00010020
#define ALT_CPU_FLASH_ACCELERATOR_LINES 0
#define ALT_CPU_FLASH_ACCELERATOR_LINE_SIZE 0
#define ALT_CPU_FLUSHDA_SUPPORTED
#define ALT_CPU_FREQ 54000000
#define ALT_CPU_HARDWARE_DIVIDE_PRESENT 0
#define ALT_CPU_HARDWARE_MULTIPLY_PRESENT 0
#define ALT_CPU_HARDWARE_MULX_PRESENT 0
#define ALT_CPU_HAS_DEBUG_CORE 1
#define ALT_CPU_HAS_DEBUG_STUB
#define ALT_CPU_HAS_ILLEGAL_INSTRUCTION_EXCEPTION
#define ALT_CPU_HAS_JMPI_INSTRUCTION
#define ALT_CPU_ICACHE_LINE_SIZE 0
#define ALT_CPU_ICACHE_LINE_SIZE_LOG2 0
#define ALT_CPU_ICACHE_SIZE 0
#define ALT_CPU_INST_ADDR_WIDTH 0x11
#define ALT_CPU_NAME "nios2_qsys_0"
#define ALT_CPU_OCI_VERSION 1
#define ALT_CPU_RESET_ADDR 0x00010000


/*
 * CPU configuration (with legacy prefix - don't use these anymore)
 *
 */

#define NIOS2_BIG_ENDIAN 0
#define NIOS2_BREAK_ADDR 0x00000820
#define NIOS2_CPU_ARCH_NIOS2_R1
#define NIOS2_CPU_FREQ 54000000u
#define NIOS2_CPU_ID_SIZE 1
#define NIOS2_CPU_ID_VALUE 0x00000000
#define NIOS2_CPU_IMPLEMENTATION "tiny"
#define NIOS2_DATA_ADDR_WIDTH 0x18
#define NIOS2_DCACHE_LINE_SIZE 0
#define NIOS2_DCACHE_LINE_SIZE_LOG2 0
#define NIOS2_DCACHE_SIZE 0
#define NIOS2_EXCEPTION_ADDR 0x00010020
#define NIOS2_FLASH_ACCELERATOR_LINES 0
#define NIOS2_FLASH_ACCELERATOR_LINE_SIZE 0
#define NIOS2_FLUSHDA_SUPPORTED
#define NIOS2_HARDWARE_DIVIDE_PRESENT 0
#define NIOS2_HARDWARE_MULTIPLY_PRESENT 0
#define NIOS2_HARDWARE_MULX_PRESENT 0
#define NIOS2_HAS_DEBUG_CORE 1
#define NIOS2_HAS_DEBUG_STUB
#define NIOS2_HAS_ILLEGAL_INSTRUCTION_EXCEPTION
#define NIOS2_HAS_JMPI_INSTRUCTION
#define NIOS2_ICACHE_LINE_SIZE 0
#define NIOS2_ICACHE_LINE_SIZE_LOG2 0
#define NIOS2_ICACHE_SIZE 0
#define NIOS2_INST_ADDR_WIDTH 0x11
#define NIOS2_OCI_VERSION 1
#define NIOS2_RESET_ADDR 0x00010000


/*
 * Define for each module class mastered by the CPU
 *
 */

#define __ALTERA_NIOS2_GEN2
#define __ALTERA_NIOS_CUSTOM_INSTR_BITSWAP
#define __ALTERA_NIOS_CUSTOM_INSTR_ENDIANCONVERTER
#define __ALTERA_AVALON_TIMER
#define __ALTERA_AVALON_JTAG_UART
#define __ALTERA_AVALON_ONCHIP_MEMORY2
#define __INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP
#define __I2C_OPENCORES
#define __SDC_CONTROLLER
#define __ALTERA_AVALON_PIO


/*
 * System configuration
 *
 */

#define ALT_DEVICE_FAMILY "Cyclone IV E"
#define ALT_ENHANCED_INTERRUPT_API_PRESENT
#define ALT_IRQ_BASE NULL
#define ALT_LOG_PORT "/dev/null"
#define ALT_LOG_PORT_BASE 0x0
#define ALT_LOG_PORT_DEV null
#define ALT_LOG_PORT_TYPE ""
#define ALT_NUM_EXTERNAL_INTERRUPT_CONTROLLERS 0
#define ALT_NUM_INTERNAL_INTERRUPT_CONTROLLERS 1
#define ALT_NUM_INTERRUPT_CONTROLLERS 1
#define ALT_SYSTEM_NAME "sys"


/*
 * hal configuration
 *
 */

#define ALT_MAX_FD 32
#define ALT_SYS_CLK none
#define ALT_TIMESTAMP_CLK TIMER_0


/*
 * timer_0 configuration
 *
 */

#define ALT_MODULE_CLASS_timer_0 altera_avalon_timer
#define TIMER_0_ALWAYS_RUN 0
#define TIMER_0_BASE 0x80000100
#define TIMER_0_COUNTER_SIZE 64
#define TIMER_0_FIXED_PERIOD 0
#define TIMER_0_FREQ 54
#define TIMER_0_IRQ 0
#define TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define TIMER_0_LOAD_VALUE 26
#define TIMER_0_MULT 1.0E-6
#define TIMER_0_NAME "/dev/timer_0"
#define TIMER_0_PERIOD 1
#define TIMER_0_PERIOD_UNITS "us"
#define TIMER_0_RESET_OUTPUT 0
#define TIMER_0_SNAPSHOT 1
#define TIMER_0_SPAN 64
#define TIMER_0_TICKS_PER_SEC 1000000
#define TIMER_0_TIMEOUT_PULSE_OUTPUT 0
#define TIMER_0_TYPE "altera_avalon_timer"


/*
 * uart_0 configuration
 *
 */

#define ALT_MODULE_CLASS_uart_0 altera_avalon_uart
#define UART_0_BASE 0x80000200

/*
 * intel_generic_serial_flash_interface_top_0_avl_csr configuration
 *
 */

#define ALT_MODULE_CLASS_intel_generic_serial_flash_interface_top_0_avl_csr intel_generic_serial_flash_interface_top
#define INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE 0x80000300
#define INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_IRQ -1
#define INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_IRQ_INTERRUPT_CONTROLLER_ID -1
#define INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_NAME "/dev/intel_generic_serial_flash_interface_top_0_avl_csr"
#define INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_SPAN 256
#define INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_TYPE "intel_generic_serial_flash_interface_top"

/*
 * intel_generic_serial_flash_interface_top_0_avl_mem configuration
 *
 */

#define ALT_MODULE_CLASS_intel_generic_serial_flash_interface_top_0_avl_mem intel_generic_serial_flash_interface_top
#define INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_MEM_BASE 0x10000000
#define INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_MEM_IRQ -1
#define INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_MEM_IRQ_INTERRUPT_CONTROLLER_ID -1
#define INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_MEM_NAME "/dev/intel_generic_serial_flash_interface_top_0_avl_mem"
#define INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_MEM_SPAN 0x200000
#define INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_MEM_TYPE "intel_generic_serial_flash_interface_top"

/*
 * onchip_memory2_0 configuration
 *
 */

#define ALT_MODULE_CLASS_onchip_memory2_0 altera_avalon_onchip_memory2
#define ONCHIP_MEMORY2_0_ALLOW_IN_SYSTEM_MEMORY_CONTENT_EDITOR 0
#define ONCHIP_MEMORY2_0_ALLOW_MRAM_SIM_CONTENTS_ONLY_FILE 0
#define ONCHIP_MEMORY2_0_BASE 0x10200000
#define ONCHIP_MEMORY2_0_CONTENTS_INFO ""
#define ONCHIP_MEMORY2_0_DUAL_PORT 0
#define ONCHIP_MEMORY2_0_GUI_RAM_BLOCK_TYPE "AUTO"
#define ONCHIP_MEMORY2_0_INIT_CONTENTS_FILE "sys_onchip_memory2_0"
#define ONCHIP_MEMORY2_0_INIT_MEM_CONTENT 1
#define ONCHIP_MEMORY2_0_INSTANCE_ID "NONE"
#define ONCHIP_MEMORY2_0_IRQ -1
#define ONCHIP_MEMORY2_0_IRQ_INTERRUPT_CONTROLLER_ID -1
#define ONCHIP_MEMORY2_0_NAME "/dev/onchip_memory2_0"
#define ONCHIP_MEMORY2_0_NON_DEFAULT_INIT_FILE_ENABLED 0
#define ONCHIP_MEMORY2_0_RAM_BLOCK_TYPE "AUTO"
#define ONCHIP_MEMORY2_0_READ_DURING_WRITE_MODE "DONT_CARE"
#define ONCHIP_MEMORY2_0_SINGLE_CLOCK_OP 0
#define ONCHIP_MEMORY2_0_SIZE_MULTIPLE 1
#define ONCHIP_MEMORY2_0_SIZE_VALUE 32768
#define ONCHIP_MEMORY2_0_SPAN 32768
#define ONCHIP_MEMORY2_0_TYPE "altera_avalon_onchip_memory2"
#define ONCHIP_MEMORY2_0_WRITABLE 1


/*
 * i2c_opencores_0 configuration
 *
 */

#define ALT_MODULE_CLASS_i2c_opencores_0 i2c_opencores
#define I2C_OPENCORES_0_BASE 0x80000500
#define I2C_OPENCORES_0_IRQ 3
#define I2C_OPENCORES_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define I2C_OPENCORES_0_NAME "/dev/i2c_opencores_0"
#define I2C_OPENCORES_0_SPAN 32
#define I2C_OPENCORES_0_TYPE "i2c_opencores"


/*
 * sdc_controller_0 configuration
 *
 */

#define ALT_MODULE_CLASS_sdc_controller_0 sdc_controller
#define SDC_CONTROLLER_QSYS_0_BASE 0x80000600
#define SDC_CONTROLLER_QSYS_0_IRQ -1
#define SDC_CONTROLLER_QSYS_0_IRQ_INTERRUPT_CONTROLLER_ID -1
#define SDC_CONTROLLER_QSYS_0_NAME "/dev/sdc_controller_0"
#define SDC_CONTROLLER_QSYS_0_SPAN 0x100
#define SDC_CONTROLLER_QSYS_0_TYPE "sdc_controller"


/*
 * sc_config configuration
 *
 */

#define ALT_MODULE_CLASS_sc_config_0 sc_config
#define SC_CONFIG_0_BASE 0x80000900
#define SC_CONFIG_0_SPAN 16


/*
 * osd_generator configuration
 *
 */

#define ALT_MODULE_CLASS_osd_generator_0 osd_generator
#define OSD_GENERATOR_0_BASE 0x80000C00
#define OSD_GENERATOR_0_SPAN 1024


/*
 * pll_reconfig configuration
 *
 */

#define ALT_MODULE_CLASS_pll_reconfig_0 pll_reconfig
#define PLL_RECONFIG_0_BASE 0x80001000
#define PLL_RECONFIG_0_SPAN 8



#define FLASH_CSR_BASE INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE
#define FLASH_MEM_BASE INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_MEM_BASE
#define I2CA_BASE I2C_OPENCORES_0_BASE
#define SDC_CONTROLLER_BASE SDC_CONTROLLER_QSYS_0_BASE
#define SCANCOVERTER_CONFIG_BASE SC_CONFIG_0_BASE
#define PLL_RECONFIG_BASE PLL_RECONFIG_0_BASE
#define OSD_GENERATOR_BASE OSD_GENERATOR_0_BASE
#define OSD_FB_BASE 0xa0000000

#define SC							((volatile sc_regs *) SCANCOVERTER_CONFIG_BASE)
#define OSD							((volatile osd_regs *) OSD_GENERATOR_BASE)
#define PLL_RECONFIG				((volatile pll_reconfig_regs *) PLL_RECONFIG_BASE)
#define OSDFB8						((uint8_t *) OSD_FB_BASE)
#define OSDFB16						((uint16_t *) OSD_FB_BASE)
#define OSDFB32						((uint32_t *) OSD_FB_BASE)

#endif /* __SYSTEM_H_ */
