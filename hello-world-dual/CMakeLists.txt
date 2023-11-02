cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(../picoSDK_import.cmake)

project(helloWorld2 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(helloWorld2
	main.c
	aux.c
)

target_include_directories(helloWorld2 PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(helloWorld2
	FreeRTOS-Kernel FreeRTOS-Kernel-Heap1
	pico_stdlib
	pico_multicore
)
pico_add_extra_outputs(helloWorld2)
pico_enable_stdio_usb(helloWorld2 1)
pico_enable_stdio_uart(helloWorld2 0)
