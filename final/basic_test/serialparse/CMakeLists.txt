# CMake minimum version required
cmake_minimum_required(VERSION 3.10)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Project name
project(serialparse  C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# Add executable target
add_executable( serialparse
   main.c )

   target_link_libraries(serialparse
   pico_stdlib
   
   )


pico_add_extra_outputs( serialparse)

pico_enable_stdio_usb(serialparse 1)
pico_enable_stdio_uart(serialparse 0)
