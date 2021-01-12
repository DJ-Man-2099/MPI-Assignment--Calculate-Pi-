cmake_minimum_required(VERSION 3.0.0)
project(Assignment4 VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(Assignment4 main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

find_package(MPI REQUIRED)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})
target_link_libraries(Assignment4 ${MPI_C_LIBRARIES})

INCLUDE_DIRECTORIES("/usr/include/x86_64-linux-gnu/gmp/")
target_link_libraries(Assignment4 gmp)

