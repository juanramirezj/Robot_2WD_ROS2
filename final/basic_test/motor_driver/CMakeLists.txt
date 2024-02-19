cmake_minimum_required(VERSION 3.12)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(mootor_driver C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(motor_driver
    main.cpp 
)

target_link_libraries(motor_driver 
   pico_stdlib
   hardware_pwm)
   
pico_add_extra_outputs( motor_driver)

