cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(../picoSDK_import.cmake)

# Pull in FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

project(usblink C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

pico_sdk_init()

add_executable(usblink
	main.c
	aux.c
)

target_include_directories(usblink PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(usblink pico_stdlib FreeRTOS-Kernel FreeRTOS-Kernel-Heap1)
pico_add_extra_outputs(usblink)
pico_enable_stdio_usb(usblink 1)
pico_enable_stdio_uart(usblink 0)
