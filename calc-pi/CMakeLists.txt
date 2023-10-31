cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(../picoSDK_import.cmake)

# Pull in FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

project(calcPi C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(calcPi
	main.c
	aux.c
)

target_include_directories(calcPi PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(calcPi pico_stdlib FreeRTOS-Kernel FreeRTOS-Kernel-Heap4)
pico_add_extra_outputs(calcPi)
pico_enable_stdio_usb(calcPi 1)
pico_enable_stdio_uart(calcPi 0)
