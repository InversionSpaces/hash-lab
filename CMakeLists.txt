cmake_minimum_required(VERSION 3.0.0)
project(hash-lab VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

include_directories(inc)
add_executable(hash-lab hash-lab.cpp)

find_package(OpenSSL REQUIRED)
target_link_libraries(hash-lab OpenSSL::SSL)

