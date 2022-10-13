PROJECT_NAME = raiju
VERSION := 1

TARGET_BOARD := target_$(PROJECT_NAME)_$(VERSION)

DEVICE_FAMILY  := STM32F1xx
DEVICE_TYPE    := STM32F103xx
DEVICE_DEF     := STM32F103xG
DEVICE         := STM32F103RF


# Linker script file without .ld extension
# Find it on cube folder after code generation
DEVICE_LD_FILE := STM32F103RFTx_FLASH

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
