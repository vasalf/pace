#include <graph/graph.h>
#include <kernels/trivial.h>

#include <iostream>

int main() {
    PaceVC::Graph graph = PaceVC::readGraph(std::cin);
    PaceVC::Kernels::Trivial(graph).reduce();

    for (int i = 0; i < graph.realSize(); i++) {
        if (!graph.undecided().count(i)) {
            continue;
        }
        if (!graph.adjacent(i).empty()) {
            graph.takeVertex(i);
        }
    }

    std::cout << graph.restoreSolution().size() << std::endl;
    return 0;
}
