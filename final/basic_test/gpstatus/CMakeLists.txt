# CMake minimum version required
cmake_minimum_required(VERSION 3.10)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Project name
project(gpstatus  C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# Add executable target
add_executable( gpstatus
   main.c )

   target_link_libraries(gpstatus
   pico_stdlib
   
   )


pico_add_extra_outputs( gpstatus)

pico_enable_stdio_usb(gpstatus 1)
pico_enable_stdio_uart(gpstatus 0)
