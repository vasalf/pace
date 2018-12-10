#include <graph/graph.h>
#include <surplus/surplus.h>

#include <iostream>

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    PaceVC::MinimalSurplusNonEmptySetFinder surplus(g);
    int ssize = surplus.find();
    std::cout << "surplus=" << ssize << std::endl;
    std::cout << "set: ";
    for (int u : surplus.set)
        std::cout << u + 1 << " ";
    std::cout << std::endl;
    std::cout << "neighbours: ";
    for (int u : surplus.neighbours)
        std::cout << u + 1 << " ";
    std::cout << std::endl;
    return 0;
}
