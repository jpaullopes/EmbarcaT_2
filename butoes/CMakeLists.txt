# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(butoes C CXX ASM)

include($ENV{FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)
# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(butoes 
    src/app_main.c
    lib/buttons_driver/buttons.c
    lib/http_client_module/http_client.c
    lib/wifi_module/wifi.c
    lib/sensor_temp/sensor_temp.c
)

pico_set_program_name(butoes "butoes")
pico_set_program_version(butoes "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(butoes 0)
pico_enable_stdio_usb(butoes 1)

# Add the standard include files to the build
target_include_directories(butoes PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${PICO_SDK_PATH}/lib/lwip/src/include
        ${PICO_SDK_PATH}/lib/lwip/src/include/arch
        ${PICO_SDK_PATH}/lib/lwip/src/include/lwip
        ${CMAKE_CURRENT_LIST_DIR}/lib/buttons_driver
        ${CMAKE_CURRENT_LIST_DIR}/lib/http_client_module
        ${CMAKE_CURRENT_LIST_DIR}/lib/wifi_module
        ${CMAKE_CURRENT_LIST_DIR}/lib/sensor_temp
        ${CMAKE_CURRENT_LIST_DIR}/config
)

target_sources(butoes PRIVATE
    ${PICO_SDK_PATH}/lib/lwip/src/apps/http/httpd.c
    ${PICO_SDK_PATH}/lib/lwip/src/apps/http/fs.c
)

# Add any user requested libraries
target_link_libraries(butoes 
        pico_stdlib
        pico_stdio
        hardware_spi
        hardware_timer
        hardware_adc
        pico_cyw43_arch_lwip_threadsafe_background
        pico_multicore
        FreeRTOS-Kernel
        FreeRTOS-Kernel-Heap4
        )

pico_add_extra_outputs(butoes)

