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
    PaceVC::Kernels::cleanUp(graph);
}

}

namespace PaceVC {
namespace Kernels {

LPKernel::LPKernel(Graph& g)
    : graph(g)
{}

void LPKernel::reduce() {
    MinimalSurplusSetFinder surplus(graph);
    surplus.find();

    reduceImpl(graph, surplus);
}

ZeroSurplusLPKernel::ZeroSurplusLPKernel(Graph& g)
    : graph(g)
{}

void ZeroSurplusLPKernel::reduce() {
    MinimalSurplusNonEmptySetFinder surplus(graph);
    surplus.find();

    reduceImpl(graph, surplus);
}

}
}
