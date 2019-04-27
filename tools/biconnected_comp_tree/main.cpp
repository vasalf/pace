#include <graph/graph.h>
#include <graph/util.h>
#include <kernels/trivial.h>

#include <iostream>

int main() {
    PaceVC::Graph graph = PaceVC::readGraph(std::cin);
    PaceVC::Kernels::Trivial(graph).reduce();
    PaceVC::BiconnectedComponentsTree compTree(graph);
    std::cout << "digraph g {" << std::endl;
    for (int i = 0; i < (int)compTree.next.size(); i++) {
        for (int v : compTree.next[i]) {
            std::cout << "    " << i << " -> " << v << ";" << std::endl;
        }
    }
    std::cout << "}" << std::endl;
    return 0;
}
