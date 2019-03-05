#include <graph/graph.h>
#include <kernels/common.h>
#include <kernels/lp/lp.h>
#include <kernels/trivial.h>

#include <iostream>

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    PaceVC::Kernels::Trivial(g).reduce();
    PaceVC::Kernels::printStats<PaceVC::Kernels::ZeroSurplusLPKernel>(g, std::cout);
    return 0;
}
