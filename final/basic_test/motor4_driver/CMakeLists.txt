cmake_minimum_required(VERSION 3.12)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(mootor3_driver C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(motor4_driver

    encoder_driver.c
    ROSPicoBridge.c
    motor_driver.c
)

pico_generate_pio_header(motor4_driver ${CMAKE_CURRENT_LIST_DIR}/quadrature_encoder.pio)

target_link_libraries(motor4_driver 
   pico_stdlib
   hardware_pwm
   pico_multicore
   hardware_pio
   
   
   )

      
pico_add_extra_outputs( motor4_driver)

pico_enable_stdio_usb(motor4_driver 1)
pico_enable_stdio_uart(motor4_driver 0)
