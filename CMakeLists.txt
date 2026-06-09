cmake_minimum_required(VERSION 3.16)
project(POK_Robot_Control CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Release)

add_executable(pok_robot 
    src/main.cpp
    src/gun_control.cpp
)

target_include_directories(pok_robot PRIVATE src)
target_compile_options(pok_robot PRIVATE -Wall -O2)

message(STATUS "POK Robot Control - структура налаштована!")