#include <graph/graph.h>
#include <kernels/trivial.h>

#include <algorithm>
#include <iostream>
#include <limits>

int main() {
    PaceVC::Graph graph = PaceVC::readGraph(std::cin);
    //PaceVC::Kernels::Trivial(graph).reduce();

    int edges = 0;
    int mind = std::numeric_limits<int>::max();
    int maxd = std::numeric_limits<int>::min();
    for (int i : graph.undecided()) {
        edges += graph.adjacent(i).size();
        mind = std::min<int>(graph.adjacent(i).size(), mind);
        maxd = std::max<int>(graph.adjacent(i).size(), maxd);
    }
    edges /= 2;

    if (graph.size() == 0) {
        mind = maxd = 0;
    }

    std::cout << "n: " << graph.size() << std::endl;
    std::cout << "m: " << edges << std::endl;
    std::cout << "mind: " << mind << std::endl;
    std::cout << "maxd: " << maxd << std::endl;
    return 0;
}
