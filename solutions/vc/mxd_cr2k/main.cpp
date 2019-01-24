#include <graph/graph.h>
#include <branching/max_degree.h>
#include <kernels/common.h>
#include <kernels/crown/crown_2k.h>

#include <iostream>

using Solution = PaceVC::MaxDegreeBranching<PaceVC::Kernels::Exhaustive<PaceVC::Kernels::Crown2kKernel>>;

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    Solution(g).branch();
    PaceVC::printSolution(std::cout, g);
    return 0;
}
