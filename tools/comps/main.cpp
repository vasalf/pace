#include <graph/graph.h>
#include <graph/util.h>

#include <iostream>

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    std::cout << PaceVC::connectedComponents(g).size() << std::endl;
    return 0;
}
