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
        doBranch();
    }

private:
    Graph& graph;

    void doBranch() {
        IntermediateReducer(graph).reduce();

        if (graph.size() == 0) {
            Graph copy = graph;
            copy.restoreSolution();
            graph.saveSolution(copy.solution());
            return;
        }

        int v = *std::max_element(graph.undecided().begin(), graph.undecided().end(),
            [this](int u, int v) {
                return graph.adjacent(u).size() < graph.adjacent(v).size();
            }
        );

        graph.placeMark();
        graph.takeVertex(v);
        doBranch();
        graph.restoreMark();

        graph.placeMark();
        auto adjCopy = graph.adjacent(v);
        for (int u : adjCopy) {
            graph.takeVertex(u);
        }
        doBranch();
        graph.restoreMark();
    }
};

}
