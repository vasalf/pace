#include <graph/graph.h>
#include <branching/max_degree.h>
#include <kernels/common.h>
#include <kernels/crown/crown.h>

#include <iostream>

using Solution = PaceVC::MaxDegreeBranching<PaceVC::Kernels::Exhaustive<PaceVC::Kernels::CrownKernel>>;

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    Solution(g).branch();
    PaceVC::printSolution(std::cout, g);
    return 0;
}
