#include <graph/graph.h>
#include <kernels/trivial.h>

#include <iostream>

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    PaceVC::Kernels::Trivial(g).reduce();
    std::cout << "graph G {" << std::endl;
    for (int u : g.undecided()) {
        for (int v : g.adjacent(u)) {
            if (u < v) {
                std::cout << "    v" << u << " -- v" << v << ";" << std::endl;
            }
        }
    }
    std::cout << "}" << std::endl;
    return 0;
}
