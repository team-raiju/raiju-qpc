# Add STM32 toolchain
set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)


# Define compiler settings
set(CMAKE_C_COMPILER                $ENV{ARM_GCC_PATH}/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER              $ENV{ARM_GCC_PATH}/arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER              ${CMAKE_C_COMPILER})
set(CMAKE_ASM_FLAGS "-x assembler-with-cpp")

set(CMAKE_OBJCOPY                   $ENV{ARM_GCC_PATH}/arm-none-eabi-objcopy)
set(CMAKE_SIZE                      $ENV{ARM_GCC_PATH}/arm-none-eabi-size)

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# STM32 Device
if (UC_NAME STREQUAL "STM32RFT6")
    set(DEVICE_DEF        "STM32F103xG")
    set(CUBE_LOCATION     "cube/STM32F103RFT6")
else()
    set(DEVICE_DEF        "STM32F103xE")
    set(CUBE_LOCATION     "cube/STM32F103RCT6")
endif()


#### Symbols definition ####
set(SYMBOLS_SYMB
    "USE_HAL_DRIVER"
    "${DEVICE_DEF}"
    "${QNASSERT}"
    "${NDEBUG}"
)


#### Core MCU flags ####
set(MCU_PARAMETERS
    -mthumb

    # This needs attention to properly set for used MCU
    -mcpu=cortex-m3

    # Hardware support for floats
    # -mfpu=fpv4-sp-d16
    # -mfloat-abi=hard
)

#### CFLAGS ####
set(CFLAGS     
    ${MCU_PARAMETERS}
    -std=gnu11 
    -Wall
    -Wextra
    -Wno-unused-parameter
    ${OPT_LEVEL}
    -g3
    -ffunction-sections -fdata-sections -fstack-usage
    --specs=nano.specs
)

#### Linker Flags ####
set(LINKER_FLAGS     
    -T${LINKER_SCRIPT_SRC}
    ${MCU_PARAMETERS}
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    --specs=nosys.specs
    -static 
    --specs=nano.specs
    -Wl,--gc-sections
    -Wl,--start-group
    -lc
    -lm
    -Wl,--end-group
)

#### LIBS ####
set(SPECIFIC_LIBS     
    ${PROJ_PATH}/${CUBE_LOCATION}/Middlewares/ST/STM32_MotionGC_Library/Lib/MotionGC_CM3_wc32_ot.a
)


#### List of source files to compile ####

file(GLOB_RECURSE SRC_FILES 
    ${PROJ_PATH}/src/*.c
    ${PROJ_PATH}/${CUBE_LOCATION}/*.c
    ${PROJ_PATH}/${CUBE_LOCATION}/*.s
)
    
list(APPEND SRC_FILES
    $ENV{QPC}/src/qf/qep_hsm.c
    $ENV{QPC}/src/qf/qep_msm.c
    $ENV{QPC}/src/qf/qf_act.c
    $ENV{QPC}/src/qf/qf_actq.c
    $ENV{QPC}/src/qf/qf_defer.c
    $ENV{QPC}/src/qf/qf_dyn.c
    $ENV{QPC}/src/qf/qf_mem.c
    $ENV{QPC}/src/qf/qf_ps.c
    $ENV{QPC}/src/qf/qf_qact.c
    $ENV{QPC}/src/qf/qf_qeq.c
    $ENV{QPC}/src/qf/qf_qmact.c
    $ENV{QPC}/src/qf/qf_time.c
    $ENV{QPC}/src/qk/qk.c 
    $ENV{QPC}/ports/arm-cm/qk/gnu/qk_port.c
   
)

# Exclude target-pc compilation files
list(FILTER SRC_FILES EXCLUDE REGEX "${PROJ_PATH}/src/bsp/target-pc/.*\.c$")


#### Include directories ####

# Search .h files
file(GLOB_RECURSE HEADER_FILES 
    ${PROJ_PATH}/inc/*.h
    ${PROJ_PATH}/${CUBE_LOCATION}/*.h
    $ENV{QPC}/include/*.h
    $ENV{QPC}/ports/arm-cm/qk/gnu/*.h
)


# Exclude target-pc compilation files
list(FILTER HEADER_FILES EXCLUDE REGEX "${PROJ_PATH}/inc/bsp/target-pc/.*\.h$")

# Append each directory path and then remove duplicates includes
foreach(HEADER_FILE ${HEADER_FILES})
    get_filename_component(HEADER_DIRECTORY ${HEADER_FILE} DIRECTORY)
    list(APPEND INCLUDE_DIRS ${HEADER_DIRECTORY})
    # message(STATUS "Header files: ${HEADER_FILE}")
endforeach()

list(REMOVE_DUPLICATES INCLUDE_DIRS)
