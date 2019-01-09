cmake_minimum_required(VERSION 3.12)
project(pace)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

include(FindZLIB)
find_package(ZLIB REQUIRED)

add_subdirectory(third_party/gtest)

add_subdirectory(third_party/abseil-cpp)

set (USE_GLOG 0)
set (USE_GFLAGS 0)
set (USE_PROTOBUF 0)

add_subdirectory(lib)
add_subdirectory(solutions)
add_subdirectory(tools)
