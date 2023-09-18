cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(../picoSDK_import.cmake)

# Pull in FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

project(blink C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(blink
	main.c
	aux.c
)

target_include_directories(blink PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(blink pico_stdlib FreeRTOS-Kernel FreeRTOS-Kernel-Heap1)
pico_add_extra_outputs(blink)
pico_enable_stdio_usb(blink 1)
pico_enable_stdio_uart(blink 0)
