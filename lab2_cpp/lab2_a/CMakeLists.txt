project(lab2_a)

set(CMAKE_CXX_STANDARD 20)
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

SET(CMAKE_C_COMPILER mpicc)
SET(CMAKE_CXX_COMPILER mpicxx)

add_executable(cars main.cpp)