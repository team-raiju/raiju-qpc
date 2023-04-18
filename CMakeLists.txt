cmake_minimum_required(VERSION 3.15.3)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(raiju)

enable_language(CXX C ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(STM_DRIVERS_FOLDER /usr/share/STM32CubeF1/Drivers)
set(QPC_FOLDER /opt/qp/qpc)

set(STM32CUBE_GENERATED_FILES
    ${STM_DRIVERS_FOLDER}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c
    ${STM_DRIVERS_FOLDER}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c
    ${STM_DRIVERS_FOLDER}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
    ${STM_DRIVERS_FOLDER}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
    ${STM_DRIVERS_FOLDER}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c
    ${STM_DRIVERS_FOLDER}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c
    ${STM_DRIVERS_FOLDER}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c
    ${STM_DRIVERS_FOLDER}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
    ${STM_DRIVERS_FOLDER}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
    ${STM_DRIVERS_FOLDER}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c
    ${STM_DRIVERS_FOLDER}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
    ${STM_DRIVERS_FOLDER}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
    ${STM_DRIVERS_FOLDER}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c
    ${STM_DRIVERS_FOLDER}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c
    ${STM_DRIVERS_FOLDER}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c
    ${STM_DRIVERS_FOLDER}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c

    cube/Src/main.c
    cube/Src/stm32f1xx_hal_msp.c
    cube/Src/stm32f1xx_it.c
    cube/Src/system_stm32f1xx.c

    cube/startup_stm32f103xg.s
)

set(QP_PORT_DIR ${QPC_FOLDER}/ports/arm-cm/qk/gnu)
# set(QP_PORT_DIR ${QPC_FOLDER}/ports/posix-qv)
# set(QP_PORT_DIR ${QPC_FOLDER}/ports/posix)

set(QP_SRCS
    ${QPC_FOLDER}/src/qf/qep_hsm.c
    ${QPC_FOLDER}/src/qf/qep_msm.c
    ${QPC_FOLDER}/src/qf/qf_act.c
    ${QPC_FOLDER}/src/qf/qf_actq.c
    ${QPC_FOLDER}/src/qf/qf_defer.c
    ${QPC_FOLDER}/src/qf/qf_dyn.c
    ${QPC_FOLDER}/src/qf/qf_mem.c
    ${QPC_FOLDER}/src/qf/qf_ps.c
    ${QPC_FOLDER}/src/qf/qf_qact.c
    ${QPC_FOLDER}/src/qf/qf_qeq.c
    ${QPC_FOLDER}/src/qf/qf_qmact.c
    ${QPC_FOLDER}/src/qf/qf_time.c
    ${QPC_FOLDER}/include/qstamp.c

    ${QPC_FOLDER}/src/qk/qk.c
    ${QP_PORT_DIR}/qk_port.c
)


set(TARGET_FOLDER target-stm32f103)

set(TARGET_FILES
    src/bsp/${TARGET_FOLDER}/bsp_adc_dma.c
    src/bsp/${TARGET_FOLDER}/bsp_ble.c
    src/bsp/${TARGET_FOLDER}/bsp_buzzer.c
    src/bsp/${TARGET_FOLDER}/bsp_eeprom.c
    src/bsp/${TARGET_FOLDER}/bsp_gpio.c
    src/bsp/${TARGET_FOLDER}/bsp_led.c
    src/bsp/${TARGET_FOLDER}/bsp_motors.c
    src/bsp/${TARGET_FOLDER}/bsp_ppm.c
    src/bsp/${TARGET_FOLDER}/bsp_uart_crsf.c
    src/bsp/${TARGET_FOLDER}/bsp_uart_radio.c
    src/bsp/${TARGET_FOLDER}/bsp_uart.c
    src/bsp/${TARGET_FOLDER}/bsp_ws2812.c
    src/bsp/${TARGET_FOLDER}/bsp.c
    src/bsp/${TARGET_FOLDER}/eeprom.c
)

set(EXECUTABLE ${PROJECT_NAME}.out)
add_executable(${EXECUTABLE}
    ${QP_SRCS}
    ${STM32CUBE_GENERATED_FILES}
    ${TARGET_FILES}

    src/main.c

    src/app.c
    src/custom_strategy.c
    src/parameters_set.c
    src/parameters.c
    src/radio_crsf.c
    src/sumo_hsm.c

    src/services/adc_service.c
    src/services/ble_service.c
    src/services/button.c
    src/services/buzzer_service.c
    src/services/distance_service.c
    src/services/driving_service.c
    src/services/led_service.c
    src/services/radio_service.c
    src/services/start_module.c
)

target_compile_definitions(${EXECUTABLE} PRIVATE
    -DUSE_HAL_DRIVER
    -DSTM32F103xG
    -DQP_API_VERSION=9999
    -DRADIO_MODE_UART_CRSF

    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(${EXECUTABLE} SYSTEM PRIVATE
    cube/Inc
    ${STM_DRIVERS_FOLDER}/STM32F1xx_HAL_Driver/Inc
    ${STM_DRIVERS_FOLDER}/STM32F1xx_HAL_Driver/Inc/Legacy
    ${STM_DRIVERS_FOLDER}/CMSIS/Device/ST/STM32F1xx/Include
    ${STM_DRIVERS_FOLDER}/CMSIS/Include
    ${QPC_FOLDER}/include
    ${QP_PORT_DIR}
)

target_include_directories(${EXECUTABLE} PRIVATE
    inc/
    inc/services
    inc/bsp
    inc/bsp/${TARGET_FOLDER}
)

target_compile_options(${EXECUTABLE} PRIVATE
    -mcpu=cortex-m3
    -mthumb

    -fdata-sections
    -ffunction-sections
    -fno-exceptions

    -Wall

    $<$<CONFIG:Debug>:-Og>
)

target_link_options(${EXECUTABLE} PRIVATE
    -T${CMAKE_SOURCE_DIR}/cube/STM32F103RFTx_FLASH.ld
    -mcpu=cortex-m3
    -mthumb
    -specs=nano.specs
    -specs=nosys.specs
    -lc
    -lm
    -lnosys
    -Wl,-Map=${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
)

add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
    COMMAND arm-none-eabi-size ${EXECUTABLE}
)

add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
    COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
)
