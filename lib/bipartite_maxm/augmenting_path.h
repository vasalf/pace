#pragma once

#include <graph/bipartite_graph.h>

#include <vector>

namespace PaceVC {

struct KuhnAugmentingPathFinder {
    const BipartiteGraph& graph;
    std::vector<bool>& visLeft;
    std::vector<bool>& visRight;
    std::vector<int>& pair;

    bool find(int v);
};

}
