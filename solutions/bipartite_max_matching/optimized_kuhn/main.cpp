#include <graph/bipartite_graph.h>
#include <bipartite_maxm/augmenting_path.h>
#include <bipartite_maxm/matching.h>

#include <iostream>

using namespace PaceVC;
using MaxMFinder = OptimizedKuhnMatchingFinder<OptimizedKuhnAugmentingPathFinder>;

int main() {
    BipartiteGraph graph = readBipartiteGraph(std::cin);
    std::cout << MaxMFinder(graph).find() << std::endl;
}
