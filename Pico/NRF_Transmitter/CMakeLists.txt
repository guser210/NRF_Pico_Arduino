cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)

set(CMAKE_CXX_STANDARD 17)

project(NRF_Transmitter)  

pico_sdk_init()
 
   
add_executable(NRF_Transmitter 
src/main.cpp
src/NRF24.cpp
#src/hm11.h
)
target_include_directories(NRF_Transmitter PUBLIC
inc
)


target_link_libraries(NRF_Transmitter
pico_stdlib
hardware_spi
hardware_uart
)

pico_add_extra_outputs(NRF_Transmitter)
