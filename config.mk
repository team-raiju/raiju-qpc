PROJECT_NAME = stm32f103kbt6-all
VERSION := 1

TARGET_BOARD := target_$(PROJECT_NAME)_$(VERSION)

DEVICE_FAMILY  := STM32G4xx
DEVICE_TYPE    := STM32G431xx
DEVICE_DEF     := STM32G431xx
DEVICE         := STM32G431KB

# Linker script file without .ld extension
# Find it on cube folder after code generation
DEVICE_LD_FILE := STM32G431KBTx_FLASH

# Lib dir
LIB_DIR  := lib

# Cube Directory
CUBE_DIR := cube

# Config Files Directory
CFG_DIR :=

# Tests Directory
TEST_DIR := tests

# Default values, can be set on the command line or here
DEBUG   ?= 1
VERBOSE ?= 0
TEST    ?= 0
