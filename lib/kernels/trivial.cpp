#include <kernels/trivial.h>

#include <queue>

namespace PaceVC {
namespace Kernels {

Trivial::Trivial(Graph& g)
    : graph(g)
{}

void Trivial::reduce() {
    Graph::Set<int> undecidedCopy = graph.undecided();

    std::queue<int> leaves;
    for (int u : undecidedCopy) {
        if (graph.adjacent(u).size() == 1)
            leaves.push(u);   
    }

    while (!leaves.empty()) {
        int a = leaves.front();
        leaves.pop();

        if (!graph.undecided().count(a))
            continue;
        
        int u = -1;
        if (graph.adjacent(a).size() == 1) {
            u = *(graph.adjacent(a).begin());
        }
        graph.removeVertex(a);

        if (u != -1) {
            auto adjCopy = graph.adjacent(u);
            graph.takeVertex(u);
            for (int v : adjCopy) {
                if (graph.adjacent(v).size() == 1) {
                    leaves.push(v);
                }
            }
        }
    }

    undecidedCopy = graph.undecided();
    for (int u : undecidedCopy) {
        if (graph.adjacent(u).empty()) {
            graph.removeVertex(u);
        }
    }
}

}
}
