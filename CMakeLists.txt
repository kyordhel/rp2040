# Top level CMakeLists.txt to build the blink Demos
cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(picoSDK_import.cmake)
include(freeRTOS_import.cmake)

PROJECT(blink)

add_subdirectory(blink)
add_subdirectory(usblink)
