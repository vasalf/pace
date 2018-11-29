include(CTest)

add_library(pacevc STATIC
    graph/boost_graph.cpp
    graph/bipartite_graph.cpp
    graph/graph.cpp

    bipartite_maxm/augmenting_path.cpp
    bipartite_maxm/best.cpp
    bipartite_maxm/matching.cpp
    bipartite_maxm/min_vc.cpp

    reader/line_reader.cpp

    surplus/surplus.cpp
)

if (BUILD_TESTING)
    enable_testing()
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIR})

    add_executable(pacevc_test
        graph/graph_ut.cpp
        bipartite_maxm/matching_ut.cpp
        bipartite_maxm/min_vc_ut.cpp
        surplus/surplus_ut.cpp
    )

    target_link_libraries(pacevc_test
        ${GTEST_BOTH_LIBRARIES} pthread pacevc)

    gtest_add_tests(pacevc_test "" AUTO)
endif()
