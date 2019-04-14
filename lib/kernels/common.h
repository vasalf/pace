#pragma once

#include <graph/graph.h>
#include <kernels/bound.h>

namespace PaceVC {
namespace Kernels {

void cleanUp(Graph& g);

template<class Kernel>
struct Exhaustive {
    Graph& graph;
    int bound = -1;
    int lowerBound = 0;
    int spans = 0;

    Exhaustive(Graph& g)
        : graph(g)
    {}

    void reduce() {
        int lastSize = graph.size();
        Kernel(graph).reduce();

        while (graph.size() < lastSize) {
            lastSize = graph.size();
            Kernel k(graph);
            k.reduce();
            bound = getKernelBound(k);
            lowerBound = getKernelLowerBound(k);
            spans += getKernelSpans(k);
        }
    }
};

void printGraphStats(Graph& g, std::ostream& out);

template<class Kernel>
void printStats(const Graph& g, std::ostream& out) {
    Graph h = g;
    Kernel(h).reduce();
    printGraphStats(h, out);
}

}
}
