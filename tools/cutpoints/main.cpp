#include <graph/graph.h>
#include <graph/util.h>
#include <kernels/trivial.h>

#include <iostream>

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    PaceVC::Kernels::Trivial(g).reduce();

    auto v = PaceVC::cutpoints(g);
    std::cout << v.size() << std::endl;
    for (int u : v)
        std::cout << u << " ";
    std::cout << std::endl;
    return 0;
}
