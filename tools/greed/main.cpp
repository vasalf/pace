#include <graph/graph.h>
#include <solution/greed.h>

#include <iostream>

int main() {
    PaceVC::Graph graph = PaceVC::readGraph(std::cin);
    PaceVC::saveGreedSolution(graph);
    std::cout << graph.bestSolution().size() << std::endl;
    return 0;
}
