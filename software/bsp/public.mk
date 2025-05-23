BSP_LINKER_SCRIPT := 

ALT_INCLUDE_DIRS += \
        $(ALT_LIBRARY_ROOT_DIR) \
        $(ALT_LIBRARY_ROOT_DIR)/drivers/inc \
		$(ALT_LIBRARY_ROOT_DIR)/HAL/inc

ALT_LIBRARY_DIRS += $(ALT_LIBRARY_ROOT_DIR)

ALT_CPPFLAGS += -pipe

# ALT_CFLAGS += -march=rv32emc_zicsr_zifencei -mabi=ilp32e
ALT_CFLAGS += -march=rv32emc_zicsr_zifencei -mabi=ilp32e -static -mcmodel=medany -fvisibility=hidden -nostdlib -nostartfiles -ffreestanding

BSP_LIB := libhal_bsp.a

ALT_LIBRARY_NAMES += m

ALT_LDDEPS += $(ALT_LIBRARY_ROOT_DIR)/$(BSP_LIB)

MAKEABLE_LIBRARY_ROOT_DIRS += $(ALT_LIBRARY_ROOT_DIR)