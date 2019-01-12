add_library(pacevc STATIC
    graph/boost_graph.cpp
    graph/bipartite_graph.cpp
    graph/graph.cpp

    bipartite_maxm/augmenting_path.cpp
    bipartite_maxm/best.cpp
    bipartite_maxm/matching.cpp
    bipartite_maxm/min_vc.cpp

    branching/max_degree.cpp

    kernels/common.cpp
    kernels/crown/crown.cpp
    kernels/lp/lp.cpp
    kernels/lp/glop.cpp

    reader/line_reader.cpp

    surplus/surplus.cpp

    util/cow.h
)

target_link_libraries(pacevc LINK_PUBLIC 
    absl::base absl::container absl::hashtablez_sampler
    absl::hash
    pthread)

if(BUILD_TESTING)
    add_executable(pacevc_test
        graph/graph_ut.cpp
        bipartite_maxm/matching_ut.cpp
        bipartite_maxm/min_vc_ut.cpp
        surplus/surplus_ut.cpp
        util/cow_ut.cpp
    )

    target_link_libraries(pacevc_test
        ${GTEST_BOTH_LIBRARIES} pthread pacevc)

    gtest_add_tests(pacevc_test "" AUTO)
endif()
