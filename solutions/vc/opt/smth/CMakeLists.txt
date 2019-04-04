add_executable(smth main.cpp)
if (DEBUG_BRANCHING)
    target_compile_definitions(smth PUBLIC DEBUG_BRANCHING)
endif()
