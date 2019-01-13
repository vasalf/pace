#pragma once

#include <graph/graph.h>

namespace PaceVC {
namespace Kernels {

struct Trivial {
    Graph& graph;

    Trivial(Graph& g);

    void reduce();
};

}
}
