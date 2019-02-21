#include <graph/graph.h>

#include <iostream>

int main() {
    PaceVC::Graph graph = PaceVC::readGraph(std::cin);

    for (int i = 0; i < graph.realSize(); i++) {
        if (!graph.undecided().count(i)) {
            continue;
        }
        if (!graph.adjacent(i).empty()) {
            graph.takeVertex(i);
        }
    }

    std::cout << graph.solution().size() << std::endl;
    return 0;
}
