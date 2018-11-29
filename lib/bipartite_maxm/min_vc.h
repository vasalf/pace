#pragma once

#include <graph/bipartite_graph.h>

#include <cassert>
#include <vector>

namespace PaceVC {

template<class MaxMFinder, class AugPathFinder>
struct MinVCFinder {
    const BipartiteGraph& graph;
    std::vector<BipartiteGraph::Vertex> answer;

    MinVCFinder(const BipartiteGraph& g) : graph(g) {}

    int find() {
        MaxMFinder maxm(graph);
        int ret = maxm.find();

        std::vector<bool> visLeft(graph.leftSize());
        std::vector<bool> visRight(graph.rightSize());
        std::vector<int> pairRight(graph.leftSize(), -1);

        for (int i = 0; i < graph.rightSize(); i++)
            if (maxm.pair[i] != -1)
                pairRight[maxm.pair[i]] = i;

        AugPathFinder augPath {
            graph,
            visLeft,
            visRight,
            maxm.pair,
            pairRight
        };

        for (int i = 0; i < graph.leftSize(); i++)
            if (pairRight[i] == -1)
                augPath.find(i);

        for (int i = 0; i < graph.leftSize(); i++)
            if (!visLeft[i])
                answer.push_back(std::make_pair(BipartiteGraph::Part::LEFT, i));

        for (int i = 0; i < graph.rightSize(); i++)
            if (visRight[i])
                answer.push_back(std::make_pair(BipartiteGraph::Part::RIGHT, i));

        assert(ret == (int)answer.size());

        return ret;
    }
};

}
