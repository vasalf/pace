cmake_minimum_required(VERSION 3.12)
project(pace)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/third_party)

add_subdirectory(solutions)
