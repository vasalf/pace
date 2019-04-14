#include <solution/greed.h>

#include <kernels/trivial.h>

namespace PaceVC {

void saveGreedSolution(Graph& graph) {
    graph.placeMark();
    Kernels::Trivial(graph).reduce();

    for (int i = 0; i < graph.realSize(); i++) {
        if (!graph.undecided().count(i)) {
            continue;
        }
        if (!graph.adjacent(i).empty()) {
            graph.takeVertex(i);
        } else {
            graph.removeVertex(i);
        }
    }

    graph.saveSolution(graph.restoreSolution());
    graph.restoreMark();
}

}
