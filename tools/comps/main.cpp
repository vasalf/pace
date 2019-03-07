#include <graph/graph.h>
#include <graph/util.h>
#include <kernels/trivial.h>

#include <iostream>

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    PaceVC::Kernels::Trivial(g).reduce();
    std::cout << PaceVC::connectedComponents(g).size() << std::endl;
    return 0;
}
