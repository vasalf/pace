#pragma once

#include <graph/graph.h>

namespace PaceVC {

struct VertexCutFinder {
    Graph& g;
    std::vector<int> answer;

    VertexCutFinder(Graph& graph);
    int find();
};

}
