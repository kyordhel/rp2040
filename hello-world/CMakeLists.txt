cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(../picoSDK_import.cmake)

# Pull in FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

project(helloWorld C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(helloWorld
	main.c
	aux.c
)

target_include_directories(helloWorld PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(helloWorld pico_stdlib FreeRTOS-Kernel FreeRTOS-Kernel-Heap1)
pico_add_extra_outputs(helloWorld)
pico_enable_stdio_usb(helloWorld 1)
pico_enable_stdio_uart(helloWorld 0)
