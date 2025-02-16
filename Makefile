# Author: M Pieklo
# Date: 16.02.2025
# Project: STM32F401CCU6_HELLO_WORLD.
# License: Opensource

include tools/makefiles/makefile_colors.mk
include tools/makefiles/makefile_info.mk
include tools/makefiles/makefile_dir.mk
include tools/makefiles/makefile_clib.mk
include tools/makefiles/makefile_common.mk

SILENTMODE := yes

NAME := $(OUT_DIR)/TARGET
NAME_STARTUP_FILE := startup_stm32f401ccux
NAME_OPENOCD_CFG := stm32f4x
DEVICE := STM32F401xC
SW_FLAG := DEFAULT
MACH := cortex-m4
FLOAT_ABI := hard
MAP  := -Wl,-Map=$(NAME).map  # Create map file
GC   := -Wl,--gc-sections     # Link for code size

CFLAGS := \
	-c \
	-mcpu=$(MACH) \
	-mthumb \
	-mfloat-abi=$(FLOAT_ABI) \
	-std=gnu11 \
	-O0 \
	-D$(DEVICE) \
	-D$(SW_FLAG) \
	$(USE_NANO) \
	-Wall \
	-Wextra \
	-ffunction-sections \
	-fdata-sections \
	-fstack-usage \
	-MMD \
	-Wfatal-errors \
	-Werror=implicit \
	-mfpu=fpv4-sp-d16 \
	-fdiagnostics-color=always

LDFLAGS := \
	-mcpu=$(MACH) \
	-mthumb \
	-mfloat-abi=$(FLOAT_ABI) \
	-mfpu=fpv4-sp-d16 \
	-T"tools/STM32F401CCUX_FLASH.ld" \
	$(MAP) \
	$(GC) \
	-static \
	$(USE_NANO) \
	-Wl,--start-group -lc -lm -Wl,--end-group

CONST := -DUSE_FULL_LL_DRIVER -DHSE_VALUE=25000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 \
	-DLSE_VALUE=32768 -DHSI_VALUE=16000000 -DLSI_VALUE=32000 -DVDD_VALUE=3300 -DUSE_FULL_ASSERT=1U -DPREFETCH_ENABLE=1 \
	-DINSTRUCTION_CACHE_ENABLE=1 -DDATA_CACHE_ENABLE=1 -DEXTERNAL_CLOCK_VALUE=12288000 $(CC_COMMON_MACRO)

INC := \
	-ICore/MAIN/inc/ \
	-IDrivers/STM32F4xx_HAL_Driver/inc/ \
	-IDrivers/CMSIS/Device/ST/STM32F4xx/Include/ \
	-IDrivers/CMSIS/Include/

SRC_CORE_DIRS := Core/MAIN/src

SRC_DRIVERS_DIR := Drivers/STM32F4xx_HAL_Driver/src

########################################################################################################################

.PHONY: all release

all: check_flags DIR ELF HEX

release : all

include tools/makefiles/makefile_dependencies.mk

include tools/makefiles/target_check_flags.mk
include tools/makefiles/target_chip.mk
include tools/makefiles/target_clean.mk
include tools/makefiles/target_dir.mk
include tools/makefiles/target_doc.mk
include tools/makefiles/target_elf.mk
include tools/makefiles/target_hex.mk