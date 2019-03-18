#include <graph/graph.h>
#include <branching/chen_liu_jia_3dvc.h>

#include <iostream>

using Solution = PaceVC::ChenLiuJia3DVC;

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    Solution(g).branch();
    PaceVC::printSolution(std::cout, g);
    return 0;
}
