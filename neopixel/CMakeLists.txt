cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(../picoSDK_import.cmake)

# Pull in FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

project(neopixel C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

pico_sdk_init()

add_executable(neopixel
	main.cpp
	neopixel.cpp
	aux.c
)

# generate PIO header "ws2812.pio.h" inside the src directory
pico_generate_pio_header(neopixel ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

target_include_directories(neopixel PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(neopixel
	pico_stdlib
	hardware_pio  # WS2812B
	FreeRTOS-Kernel
	FreeRTOS-Kernel-Heap1)
pico_add_extra_outputs(neopixel)
pico_enable_stdio_usb(neopixel 1)
pico_enable_stdio_uart(neopixel 0)
