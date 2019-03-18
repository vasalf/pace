#include <graph/graph.h>
#include <branching/cutpoints.h>
#include <kernels/common.h>
#include <kernels/lp/lp.h>

#include <iostream>

using Solution = PaceVC::CutpointsBranching<PaceVC::Kernels::ZeroSurplusLPKernel>;

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    Solution(g).branch();
    PaceVC::printSolution(std::cout, g);
    return 0;
}
