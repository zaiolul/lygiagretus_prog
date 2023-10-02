cmake_minimum_required(VERSION 3.12)
project(lab1_a)

set(CMAKE_CXX_STANDARD 20)

add_executable(cars main.cpp DataMonitor.cpp DataMonitor.h ResultsMonitor.cpp ResultsMonitor.h)
TARGET_LINK_LIBRARIES(cars pthread)