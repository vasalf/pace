#pragma once

#include <graph/bipartite_graph.h>

namespace PaceVC {

template<class AugmentingPathFinder>
struct ClassicKuhnMatchingFinder {
    const BipartiteGraph& graph;
    std::vector<int> pair;

    ClassicKuhnMatchingFinder(const BipartiteGraph& g)
        : graph(g), pair(g.leftSize() + g.rightSize()) {}

    int find() {
        std::vector<bool> visLeft(graph.leftSize()), visRight(graph.rightSize());
        AugmentingPathFinder aug { graph, visLeft, visRight, pair };
        int ans = 0;
        for (int v = 0; v < graph.leftSize(); v++) {
            visLeft.assign(graph.leftSize(), false);
            ans += aug.find(v);
        }
        return ans;
    }
};

}
