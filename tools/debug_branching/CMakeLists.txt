add_executable(debug_branching main.cpp)
target_link_libraries(debug_branching LINK_PUBLIC pacevc)

if (BUILD_PROFILE)
    target_link_libraries(debug_branching LINK_PUBLIC ${GPERFTOOLS_PROFILER})
endif()
