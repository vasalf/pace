#pragma once

#include <graph/graph.h>

namespace PaceVC {
namespace Kernels {

struct LPKernel {
    Graph& graph;

    LPKernel(Graph& g);

    void reduce();
};

struct ZeroSurplusLPKernel {
    Graph& graph;

    ZeroSurplusLPKernel(Graph& g);

    void reduce();
};

}
}
