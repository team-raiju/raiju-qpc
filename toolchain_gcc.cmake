# Define compiler settings
set(CMAKE_C_COMPILER                gcc)
set(CMAKE_CXX_COMPILER              g++)
set(CMAKE_ASM_COMPILER              ${CMAKE_C_COMPILER})
set(CMAKE_ASM_FLAGS "-x assembler-with-cpp")

set(CMAKE_OBJCOPY                   objcopy)
set(CMAKE_SIZE                      size)

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)


#### Symbols definition ####
set(SYMBOLS_SYMB
    "Q_SPY"
    "${QNASSERT}"
)



#### CFLAGS ####
set(CFLAGS     
    -std=gnu11 
    -W
    -Wall
    -Wextra
    -Wno-unused-parameter
    -Wno-type-limits
    -Wno-unused-result
    ${OPT_LEVEL}
    -g3
    -fno-pie
)

#### Linker Flags ####
set(LINKER_FLAGS     
    -no-pie
)

#### LIBS ####
set(SPECIFIC_LIBS     
    pthread
)

#### List of source files to compile ####

file(GLOB_RECURSE SRC_FILES 
    ${PROJ_PATH}/src/*.c
)

list(APPEND SRC_FILES
    $ENV{QPC}/src/qs/qs.c
	$ENV{QPC}/src/qs/qs_rx.c
	$ENV{QPC}/src/qs/qs_fp.c
	$ENV{QPC}/src/qs/qs_64bit.c
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
    $ENV{QPC}/include/qstamp.c
	$ENV{QPC}/ports/posix-qv/qs_port.c
	$ENV{QPC}/ports/posix-qv/qf_port.c
)

# Exclude target-pc compilation files
list(FILTER SRC_FILES EXCLUDE REGEX "${PROJ_PATH}/src/bsp/target-stm32f103/.*\.c$")


#### Include directories ####

# Search .h files
file(GLOB_RECURSE HEADER_FILES 
    ${PROJ_PATH}/inc/*.h
    $ENV{QPC}/include/*.h
    $ENV{QPC}/ports/posix-qv/*.h
)


# Exclude target-pc compilation files
list(FILTER SRC_FILES EXCLUDE REGEX "${PROJ_PATH}/inc/bsp/target-stm32f103/.*\.h$")

# Append each directory path and then remove duplicates includes
foreach(HEADER_FILE ${HEADER_FILES})
    get_filename_component(HEADER_DIRECTORY ${HEADER_FILE} DIRECTORY)
    list(APPEND INCLUDE_DIRS ${HEADER_DIRECTORY})
endforeach()

list(REMOVE_DUPLICATES INCLUDE_DIRS)
