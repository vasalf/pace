#include <kernels/common.h>

namespace PaceVC {
namespace Kernels {

void cleanUp(Graph& graph) {
    for (int u : graph.undecided())
        if (graph.adjacent(u).empty())
            graph.removeVertex(u);

    if (graph.size() < graph.realSize())
        graph.squeeze();
}

}
}
