#include <graph/graph.h>

#include <algorithm>
#include <iostream>
#include <limits>

int main() {
    PaceVC::Graph graph = PaceVC::readGraph(std::cin);
    
    int edges = 0;
    int mind = std::numeric_limits<int>::max();
    int maxd = std::numeric_limits<int>::min();
    for (int i : graph.undecided()) {
        edges += graph.adjacent(i).size();
        mind = std::min<int>(graph.adjacent(i).size(), mind);
        maxd = std::max<int>(graph.adjacent(i).size(), maxd);
    }

    std::cout << "n: " << graph.size() << std::endl;
    std::cout << "m: " << edges << std::endl;
    std::cout << "mind: " << mind << std::endl;
    std::cout << "maxd: " << maxd << std::endl;
    return 0;
}
