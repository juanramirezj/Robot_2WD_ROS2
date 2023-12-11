cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(mootor6_ROSPicoBridge C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

link_directories( /home/administrator/micro_ros_ws/src/micro_ros_raspberrypi_pico_sdk/libmicroros)
add_executable(mootor6_ROSPicoBridge
    main.c
    pico_uart_transport.c
    motor.c
    ssd1306.c
)

target_link_libraries(mootor6_ROSPicoBridge
   pico_stdlib
   microros
   hardware_pwm
   pico_stdlib 
   hardware_i2c
   )


target_include_directories(mootor6_ROSPicoBridge PUBLIC
    /home/administrator/micro_ros_ws/src/micro_ros_raspberrypi_pico_sdk/libmicroros/include
)

SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")

# Configure Pico
pico_enable_stdio_usb(mootor6_ROSPicoBridge 1)
pico_enable_stdio_uart(mootor6_ROSPicoBridge 0)
add_compile_definitions(PICO_UART_ENABLE_CRLF_SUPPORT=0)
add_compile_definitions(PICO_STDIO_ENABLE_CRLF_SUPPORT=0)
add_compile_definitions(PICO_STDIO_DEFAULT_CRLF=0)

# Generate UF2
pico_add_extra_outputs( mootor6_ROSPicoBridge)
