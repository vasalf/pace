#pragma once

#include <graph/graph.h>
#include <graph/util.h>
#include <kernels/bound.h>
#include <kernels/trivial.h>

#include <vector>

namespace PaceVC {

template<class IntermediateReducer, class VertexSelector>
class Branching {
    using TThis = Branching<IntermediateReducer, VertexSelector>;

public:

    Branching(Graph& g)
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
        graph.trySqueeze();

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

        if (graph.restoreSolution().size() >= graph.bestSolution().size()) {
            return;
        }

        auto components = connectedComponents(graph);
        if (components.size() > 1) {
            std::vector<int> indexInComponent(graph.realSize());
            for (int i = 0; i < (int)components.size(); i++) {
                for (int j = 0; j < (int)components[i].size(); j++) {
                    indexInComponent[components[i][j]] = j;
                }
            }

            for (const auto& component : components) {
                Graph g(component.size());
                for (int u : component) {
                    for (int v : graph.adjacent(u)) {
                        g.addEdge(indexInComponent[u], indexInComponent[v]);
                    }
                }

                TThis(g).branch();

                for (int u : g.bestSolution()) {
                    graph.takeVertex(component[u]);
                }
            }

            graph.saveSolution(graph.restoreSolution());
            return;
        }

        int v = VertexSelector(graph).select();

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
