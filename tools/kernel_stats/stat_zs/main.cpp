#include <graph/graph.h>
#include <kernels/common.h>
#include <kernels/lp/lp.h>

#include <iostream>

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    PaceVC::Kernels::printStats<PaceVC::Kernels::ZeroSurplusLPKernel>(g, std::cout);
    return 0;
}
