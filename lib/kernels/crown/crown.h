#pragma once

#include <graph/graph.h>

namespace PaceVC {
namespace Kernels {

struct CrownKernel {
    Graph& graph;

    CrownKernel(Graph& g);

    void reduce();
};

}
}
