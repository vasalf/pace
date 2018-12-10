#include <graph/graph.h>
#include <branching/max_degree.h>
#include <kernels/lp/lp.h>

#include <iostream>

using Solution = PaceVC::MaxDegreeBranching<PaceVC::Kernels::ExhaustiveZeroSurplusLPKernel>;

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    Solution(g).branch();
    PaceVC::printSolution(std::cout, g);
    return 0;
}
