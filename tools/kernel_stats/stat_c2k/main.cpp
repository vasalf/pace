#include <graph/graph.h>
#include <kernels/common.h>
#include <kernels/crown/crown_2k.h>
#include <kernels/trivial.h>

#include <iostream>

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    PaceVC::Kernels::Trivial(g).reduce();
    PaceVC::Kernels::printStats<PaceVC::Kernels::Exhaustive<PaceVC::Kernels::Crown2kKernel>>(g, std::cout);
    return 0;
}
