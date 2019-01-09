#pragma once

#include <graph/graph.h>

namespace PaceVC {
namespace Kernels {

struct GlopLPKernel {
    Graph& graph;

    GlopLPKernel(Graph& g);

    void reduce();
};

}
}
