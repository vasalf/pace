#include <graph/graph.h>
#include <surplus/surplus.h>

#include <iostream>

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    std::cout << 1.0 * findLPVCValue(g) / 2 << std::endl;
    return 0;
}
