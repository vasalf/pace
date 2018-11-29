#include <graph/graph.h>
#include <kernels/crown/crown.h>

#include <iostream>

int main() {
    PaceVC::Graph graph = PaceVC::readGraph(std::cin);
    PaceVC::Kernels::CrownKernel(graph).reduce();
    std::cout << graph.size() << std::endl;
    return 0;
}
