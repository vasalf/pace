#include <kernels/common.h>

namespace PaceVC {
namespace Kernels {

void cleanUp(Graph& graph) {
    Graph::Set<int> undecidedCopy = graph.undecided();
    for (int u : undecidedCopy)
        if (graph.adjacent(u).empty())
            graph.removeVertex(u);

    if (graph.size() < graph.realSize())
        graph.squeeze();
}

}
}
