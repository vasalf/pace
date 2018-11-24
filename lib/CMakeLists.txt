add_library(pacevc STATIC
    graph/boost_graph.cpp
    graph/bipartite_graph.cpp

    bipartite_maxm/augmenting_path.cpp
    bipartite_maxm/matching.cpp

    reader/line_reader.cpp
)
