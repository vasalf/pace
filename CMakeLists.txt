cmake_minimum_required(VERSION 3.12)
project(pace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DUSE_ABSL_HASH_TABLES)

include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/third_party)
include_directories(${CMAKE_SOURCE_DIR}/third_party/abseil-cpp)

include(FindZLIB)
find_package(ZLIB REQUIRED)

add_subdirectory(third_party/gtest)

add_subdirectory(third_party/abseil-cpp)

set(USE_GLOG 0)
set(USE_GFLAGS 0)
set(USE_PROTOBUF 0)

include(CTest)
if (BUILD_TESTING)
    find_package(GTest REQUIRED)
    enable_testing()
endif()

add_subdirectory(lib)
add_subdirectory(solutions)
add_subdirectory(tools)
