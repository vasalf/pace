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
    int currentSpans = 0;

    void doBranch(int bound) {
        graph.trySqueeze();

        int lowerBound = 0;

        if (bound >= graph.size() || bound < 0) {
            IntermediateReducer k(graph);
            k.reduce();
            bound = Kernels::getKernelBound(k);
            currentSpans += Kernels::getKernelSpans(k);
            lowerBound = graph.restoreSolution().size() + Kernels::getKernelLowerBound(k) + currentSpans;
        }

        Kernels::Trivial trivial(graph);
        trivial.reduce();
        currentSpans += Kernels::getKernelSpans(trivial);

        if (graph.size() == 0) {
            graph.saveSolution(graph.restoreSolution());
            return;
        }

        if (std::max<int>(graph.restoreSolution().size(), lowerBound) >= graph.bestSolution().size()) {
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
                for (int u : component) {
                    if (graph.undecided().count(u))
                        graph.removeVertex(u);
                }
            }

            graph.saveSolution(graph.restoreSolution());
            return;
        }

        int v = VertexSelector(graph).select();

        graph.placeMark();
        graph.takeVertex(v);
        int oldSpans = currentSpans;
        doBranch(bound);
        currentSpans = oldSpans;
        graph.restoreMark();

        graph.placeMark();
        auto adjCopy = graph.adjacent(v);
        for (int u : adjCopy) {
            graph.takeVertex(u);
        }
        oldSpans = currentSpans;
        doBranch(bound);
        currentSpans = oldSpans;
        graph.restoreMark();
    }
};

}
