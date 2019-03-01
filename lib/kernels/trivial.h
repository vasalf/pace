#pragma once

#include <graph/graph.h>
#include <kernels/common.h>

namespace PaceVC {
namespace Kernels {

struct TrivialImpl {
    Graph& graph;

    TrivialImpl(Graph& g);

    void reduce();
};

using Trivial = Exhaustive<TrivialImpl>;

}
}
