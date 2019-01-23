#include <graph/graph.h>
#include <kernels/crown/crown_2k.h>
#include <kernels/common.h>

#include <iostream>

int main() {
    PaceVC::Graph graph = PaceVC::readGraph(std::cin);
    PaceVC::Kernels::Exhaustive<PaceVC::Kernels::Crown2kKernel>(graph).reduce();
    std::cout << graph.size() << std::endl;
    return 0;
}
