#pragma once

#include <graph/bipartite_graph.h>

namespace PaceVC {

template<class AugmentingPathFinder>
struct ClassicKuhnMatchingFinder {
    const BipartiteGraph& graph;
    std::vector<int> pair;

    ClassicKuhnMatchingFinder(const BipartiteGraph& g)
        : graph(g), pair(g.rightSize(), -1) {}

    int find() {
        std::vector<bool> visLeft(graph.leftSize()), visRight(graph.rightSize());
        std::vector<int> rpair(graph.leftSize(), -1);
        AugmentingPathFinder aug { graph, visLeft, visRight, pair, rpair };
        int ans = 0;
        for (int v = 0; v < graph.leftSize(); v++) {
            visLeft.assign(graph.leftSize(), false);
            ans += aug.find(v);
        }
        return ans;
    }
};

template<class AugmentingPathFinder>
struct OptimizedKuhnMatchingFinder {
    const BipartiteGraph& graph;
    std::vector<int> pair;

    OptimizedKuhnMatchingFinder(const BipartiteGraph& g)
        : graph(g), pair(g.rightSize(), -1) {}

    int find() {
        std::vector<bool> visLeft, visRight(graph.rightSize());
        std::vector<int> rpair(graph.leftSize(), -1);
        AugmentingPathFinder aug { graph, visLeft, visRight, pair, rpair };
        int ans = 0;
        for (int run = 1; run; ) {
            run = 0;
            visLeft.assign(graph.leftSize(), false);
            for (int i = 0; i < graph.leftSize(); i++)
                if (rpair[i] == -1 && aug.find(i)) {
                    ans++;
                    run = 1;
                }
        }
        return ans;
    }
};

struct HopcroftKarpMatchingFinder {
    const BipartiteGraph& graph;
    std::vector<int> pair, pairRight;
    std::vector<int> dist;
    std::vector<int> edgeptr;

    HopcroftKarpMatchingFinder(const BipartiteGraph& g);

    bool bfs();
    bool dfs(int u);

    int find();
};

}
