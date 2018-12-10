#include <graph/graph.h>
#include <kernels/lp/lp.h>

#include <iostream>

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    PaceVC::Kernels::ExhaustiveZeroSurplusLPKernel(g).reduce();
    std::cout << g.size() << std::endl;
    return 0;
}
