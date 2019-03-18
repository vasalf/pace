#pragma once

#include <graph/graph.h>
#include <kernels/common.h>

namespace PaceVC {
namespace Kernels {

void handleIsolated(Graph& graph);
void handleLeaves(Graph& graph);

struct TrivialImpl {
    Graph& graph;

    TrivialImpl(Graph& g);

    void reduce();
};

using Trivial = Exhaustive<TrivialImpl>;

}
}
