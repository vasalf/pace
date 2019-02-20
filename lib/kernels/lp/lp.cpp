#include <kernels/lp/lp.h>

#include <kernels/common.h>
#include <surplus/surplus.h>

namespace {

template<class SurplusFinder>
void reduceImpl(PaceVC::Graph& graph, const SurplusFinder& surplus) {
    for (int u : surplus.set)
        graph.removeVertex(u);
    for (int v : surplus.neighbours)
        graph.takeVertex(v);
}

}

namespace PaceVC {
namespace Kernels {

LPKernel::LPKernel(Graph& g)
    : graph(g)
{}

void LPKernel::reduce() {
    MinimalSurplusSetFinder surplus(graph);
    int val = surplus.find();

    if (val > 0) {
        bound = graph.size() - val;
        return;
    }

    bound = -1;
    reduceImpl(graph, surplus);
}

ZeroSurplusLPKernel::ZeroSurplusLPKernel(Graph& g)
    : graph(g)
{}

void ZeroSurplusLPKernel::reduce() {
    MinimalSurplusSetFinder surplus(graph);
    int val = surplus.find();

    if (val > 0) {
        bound = graph.size() - val;
        return;
    }

    bound = -1;

    if (val < 0) {
        reduceImpl(graph, surplus);
        return;
    }

    auto undecidedCopy = graph.undecided();
    for (int u : undecidedCopy) {
        if (!graph.undecided().count(u))
            continue;

        Graph copy = graph;
        for (int v : graph.adjacent(u))
            copy.takeVertex(v);
        copy.removeVertex(u);
        MinimalSurplusSetFinder surplusCopy(copy);
        int copyVal = surplusCopy.find();

        if (copyVal + graph.adjacent(u).size() - 1 == 0) {
            auto adjCopy = graph.adjacent(u);
            for (int v : adjCopy)
                graph.takeVertex(v);
            graph.removeVertex(u);
            reduceImpl(graph, surplusCopy);
        }
    }
}

}
}
