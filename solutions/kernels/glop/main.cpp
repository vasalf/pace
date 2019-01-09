#include <graph/graph.h>
#include <kernels/lp/glop.h>

#include <iostream>

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    PaceVC::Kernels::GlopLPKernel(g).reduce();
    std::cout << g.size() << std::endl;
    return 0;
}
