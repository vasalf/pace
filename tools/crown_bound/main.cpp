#include <graph/graph.h>
#include <kernels/crown/crown.h>
#include <kernels/crown/crown_2k.h>

#include <iostream>

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    PaceVC::Graph h = g;

    PaceVC::Kernels::CrownKernel(g).reduce();
    PaceVC::Kernels::Crown2kKernel(h).reduce();
    std::cerr << h.size() << " " << h.solution().size() << std::endl;
    std::cout << std::max((h.size() + 1) / 2 + h.solution().size(), (g.size() + 2) / 3 + g.solution().size()) << std::endl;
    return 0;
}
