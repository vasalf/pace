#pragma once

#include <graph/graph.h>
#include <kernels/bound.h>
#include <kernels/trivial.h>

#include <algorithm>

namespace PaceVC {

template<class IntermediateReducer>
class MaxDegreeBranching {
public:

    MaxDegreeBranching(Graph& g)
        : graph(g)
    {}

    void branch() {
        graph.placeMark();
        doBranch(graph.size());
        graph.restoreMark();
    }

private:
    Graph& graph;

    void doBranch(int bound) {
        if (bound >= graph.size() || bound < 0) {
            IntermediateReducer k(graph);
            k.reduce();
            bound = Kernels::getKernelBound(k);
        }

        Kernels::Trivial(graph).reduce();

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
        doBranch(bound);
        graph.restoreMark();

        graph.placeMark();
        auto adjCopy = graph.adjacent(v);
        for (int u : adjCopy) {
            graph.takeVertex(u);
        }
        doBranch(bound);
        graph.restoreMark();
    }
};

}
