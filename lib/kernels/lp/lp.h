#pragma once

#include <graph/graph.h>
#include <kernels/common.h>

namespace PaceVC {
namespace Kernels {

struct LPKernel {
    Graph& graph;
    int bound = -1;

    LPKernel(Graph& g);

    void reduce();
};

struct ZeroSurplusLPKernel {
    Graph& graph;
    int bound = -1;

    ZeroSurplusLPKernel(Graph& g);

    void reduce();
};

using ExhaustiveZeroSurplusLPKernel = Exhaustive<ZeroSurplusLPKernel>;

}
}
