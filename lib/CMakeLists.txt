add_library(pacevc STATIC
    graph/boost_graph.cpp
    graph/bipartite_graph.cpp
    graph/graph.cpp
    graph/util.cpp

    bipartite_maxm/augmenting_path.cpp
    bipartite_maxm/best.cpp
    bipartite_maxm/matching.cpp
    bipartite_maxm/min_vc.cpp

    branching/branching.cpp
    branching/chen_liu_jia_3dvc.cpp
    branching/cutpoints.cpp
    branching/debug_branching.cpp
    branching/max_degree.cpp

    kernels/bound.cpp
    kernels/common.cpp
    kernels/crown/crown.cpp
    kernels/crown/crown_2k.cpp
    kernels/lp/lp.cpp
    kernels/lp/glop.cpp
    kernels/trivial.cpp

    reader/line_reader.cpp

    solution/greed.cpp
    solution/solution.cpp

    surplus/surplus.cpp

    util/cow.h
)

target_link_libraries(pacevc LINK_PUBLIC
    absl::base absl::container
    absl::hash absl::node_hash_set
    pthread)

if(BUILD_TESTING)
    add_executable(pacevc_test
        graph/graph_ut.cpp
        graph/util_ut.cpp
        bipartite_maxm/matching_ut.cpp
        bipartite_maxm/min_vc_ut.cpp
        branching/max_degree_ut.cpp
        branching/cutpoints_ut.cpp
        kernels/trivial_ut.cpp
        surplus/surplus_ut.cpp
        ut/vc_exact_public_ut.cpp
        util/cow_ut.cpp
    )

    target_compile_definitions(pacevc_test PUBLIC SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\")

    target_link_libraries(pacevc_test
        ${GTEST_BOTH_LIBRARIES} pthread pacevc stdc++fs)

    gtest_add_tests(pacevc_test "" AUTO)
endif()
