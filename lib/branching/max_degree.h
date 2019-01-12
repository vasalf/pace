#pragma once

#include <graph/graph.h>

#include <algorithm>

namespace PaceVC {

template<class IntermediateReducer>
class MaxDegreeBranching {
public:

    MaxDegreeBranching(Graph& g)
        : graph(g)
    {}

    void branch() {
        IntermediateReducer(graph).reduce();

        if (graph.size() == 0) {
            graph.saveSolution(graph.restoreSolution());
            return;
        }

        int v = *std::max_element(graph.undecided().begin(), graph.undecided().end(),
            [this](int u, int v) {
                return graph.adjacent(u).size() < graph.adjacent(v).size();
            }
        );

        graph.placeMark();
        graph.takeVertex(v);
        branch();
        graph.restoreMark();

        graph.placeMark();
        auto adjCopy = graph.adjacent(v);
        for (int u : adjCopy) {
            graph.takeVertex(u);
        }
        branch();
        graph.restoreMark();
    }

private:
    Graph& graph;
};

}
