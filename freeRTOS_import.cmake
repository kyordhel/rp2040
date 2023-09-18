# Based on <PICO_SDK_PATH>/external/pico_sdk_import.cmake

# This can be dropped into an external project to help locate this SDK
# It should be include()ed prior to project()

if (DEFINED ENV{FREERTOS_KERNEL_PATH} AND (NOT FREERTOS_KERNEL_PATH))
    set(FREERTOS_KERNEL_PATH $ENV{FREERTOS_KERNEL_PATH})
    message("Using FREERTOS_KERNEL_PATH from environment ('${FREERTOS_KERNEL_PATH}')")
endif ()

if (DEFINED ENV{FREERTOS_FETCH_FROM_GIT} AND (NOT FREERTOS_FETCH_FROM_GIT))
    set(FREERTOS_FETCH_FROM_GIT $ENV{FREERTOS_FETCH_FROM_GIT})
    message("Using FREERTOS_FETCH_FROM_GIT from environment ('${FREERTOS_FETCH_FROM_GIT}')")
endif ()

if (DEFINED ENV{FREERTOS_FETCH_FROM_GIT_PATH} AND (NOT FREERTOS_FETCH_FROM_GIT_PATH))
    set(FREERTOS_FETCH_FROM_GIT_PATH $ENV{FREERTOS_FETCH_FROM_GIT_PATH})
    message("Using FREERTOS_FETCH_FROM_GIT_PATH from environment ('${FREERTOS_FETCH_FROM_GIT_PATH}')")
endif ()

set(FREERTOS_KERNEL_PATH "${FREERTOS_KERNEL_PATH}" CACHE PATH "Path to the FreeRTOS kernel")
set(FREERTOS_FETCH_FROM_GIT "${FREERTOS_FETCH_FROM_GIT}" CACHE BOOL "Set to ON to fetch copy of FreeRTOS from git if not otherwise locatable")
set(FREERTOS_FETCH_FROM_GIT_PATH "${FREERTOS_FETCH_FROM_GIT_PATH}" CACHE FILEPATH "location to download FreeRTOS")

if (NOT FREERTOS_KERNEL_PATH)
    if (FREERTOS_FETCH_FROM_GIT)
        include(FetchContent)
        set(FETCHCONTENT_BASE_DIR_SAVE ${FETCHCONTENT_BASE_DIR})
        if (FREERTOS_FETCH_FROM_GIT_PATH)
            get_filename_component(FETCHCONTENT_BASE_DIR "${FREERTOS_FETCH_FROM_GIT_PATH}" REALPATH BASE_DIR "${CMAKE_SOURCE_DIR}")
        endif ()
        FetchContent_Declare(
                freertos
                GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel
                GIT_TAG V10.4.5
        )
        if (NOT freertos)
            message("Downloading FreeRTOS source")
            FetchContent_Populate(freertos)
            set(FREERTOS_KERNEL_PATH ${freertos_SOURCE_DIR})
        endif ()
        set(FETCHCONTENT_BASE_DIR ${FETCHCONTENT_BASE_DIR_SAVE})
    else ()
        message(FATAL_ERROR
                "FreeRTOS location was not specified. Please set FREERTOS_KERNEL_PATH or set FREERTOS_FETCH_FROM_GIT to on to fetch from git."
                )
    endif ()
endif ()

get_filename_component(FREERTOS_KERNEL_PATH "${FREERTOS_KERNEL_PATH}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
if (NOT EXISTS ${FREERTOS_KERNEL_PATH})
    message(FATAL_ERROR "Directory '${FREERTOS_KERNEL_PATH}' not found")
endif ()

set(TASKS_C_FILE ${FREERTOS_KERNEL_PATH}/tasks.c)
if (NOT EXISTS ${TASKS_C_FILE})
    message(FATAL_ERROR "Directory '${FREERTOS_KERNEL_PATH}' does not appear to contain the FreeRTOS kernel")
endif ()

set(FREERTOS_KERNEL_PATH ${FREERTOS_KERNEL_PATH} CACHE PATH "Path to the FreeRTOS kernel" FORCE)

# include(${FREERTOS_KERNEL_PATH}/CMakeLists.txt)
