cmake_minimum_required(VERSION 3.12)

set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(midi_router_pico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(midi_router_pico
    src/main.c
    lib/pico_pio_usb/src/pio_usb.c
)

pico_generate_pio_header(midi_router_pico ${CMAKE_CURRENT_LIST_DIR}/lib/pico_pio_usb/src/usb_rx.pio)
pico_generate_pio_header(midi_router_pico ${CMAKE_CURRENT_LIST_DIR}/lib/pico_pio_usb/src/usb_tx.pio)

target_include_directories(midi_router_pico PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/lib/pico_pio_usb/src
    ${PICO_SDK_PATH}/src/rp2_common/pico_stdlib/include
    ${PICO_SDK_PATH}/src/common/pico_base/include
    ${PICO_SDK_PATH}/src/rp2_common/hardware_dma/include
    ${PICO_SDK_PATH}/lib/tinyusb/src
)

target_link_libraries(midi_router_pico
    pico_stdlib
    pico_multicore
    hardware_pio
    hardware_dma
    hardware_uart
    tinyusb_host
    tinyusb_device
)

pico_enable_stdio_usb(midi_router_pico 0)
pico_enable_stdio_uart(midi_router_pico 1)

pico_add_extra_outputs(midi_router_pico)
