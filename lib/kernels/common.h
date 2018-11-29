#pragma once

#include <graph/graph.h>

namespace PaceVC {
namespace Kernels {

void cleanUp(Graph& g);

template<class Kernel>
struct Exhaustive {
    Graph& graph;

    Exhaustive(Graph& g)
        : graph(g)
    {}

    void reduce() {
        int lastSize = graph.size();
        Kernel(graph).reduce();

        while (graph.size() < lastSize) {
            lastSize = graph.size();
            Kernel(graph).reduce();
        }
    }
};

}
}
