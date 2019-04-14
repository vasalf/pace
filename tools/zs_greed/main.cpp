#include <graph/graph.h>
#include <kernels/lp/lp.h>
#include <solution/greed.h>

#include <iostream>

int main() {
    PaceVC::Graph graph = PaceVC::readGraph(std::cin);
    PaceVC::Kernels::ZeroSurplusLPKernel(graph).reduce();
    PaceVC::saveGreedSolution(graph);
    std::cout << graph.restoreSolution().size() << std::endl;
    return 0;
}
