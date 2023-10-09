cmake_minimum_required(VERSION 3.12)
project(lab1_b)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fopenmp")
add_executable(cars main.cpp)
TARGET_LINK_LIBRARIES(cars pthread)