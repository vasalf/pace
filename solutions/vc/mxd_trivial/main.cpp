#include <graph/graph.h>
#include <branching/max_degree.h>

#include <iostream>

namespace {

struct DoNothingKernel {
    DoNothingKernel(PaceVC::Graph&) {}
    void reduce() {}
};

}

using Solution = PaceVC::MaxDegreeBranching<DoNothingKernel>;

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    Solution(g).branch();
    PaceVC::printSolution(std::cout, g);
    return 0;
}
