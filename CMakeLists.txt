cmake_minimum_required(VERSION 3.23)
project(spi_stm_atmega_light C)

set(CMAKE_C_STANDARD 11)
add_executable(spi_stm_atmega_light main.c)

include_directories(../../SPL_stm32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x)
include_directories(../../SPL_stm32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport)
include_directories(../../SPL_stm32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/inc)