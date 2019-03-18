#pragma once

#include <graph/graph.h>

namespace PaceVC {
namespace Kernels {

struct CrownKernel {
    Graph& graph;
    int lowerBound = 0;

    CrownKernel(Graph& g);

    void reduce();
};

}
}
