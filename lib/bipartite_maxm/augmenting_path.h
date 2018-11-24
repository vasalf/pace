#pragma once

#include <graph/bipartite_graph.h>

#include <vector>

namespace PaceVC {

struct KuhnAugmentingPathFinder {
    const BipartiteGraph& graph;
    std::vector<bool>& visLeft;
    std::vector<bool>& visRight;
    std::vector<int>& pair;
    std::vector<int>& pairRight;

    bool find(int v);
};

struct OptimizedKuhnAugmentingPathFinder {
    const BipartiteGraph& graph;
    std::vector<bool>& visLeft;
    std::vector<bool>& visRight;
    std::vector<int>& pair;
    std::vector<int>& pairRight;

    bool find(int v);
};

}
