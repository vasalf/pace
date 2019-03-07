#include <graph/graph.h>
#include <branching/cutpoints.h>

#include <iostream>

namespace {

struct DoNothingKernel {
    DoNothingKernel(PaceVC::Graph&) {}
    void reduce() {}
};

}

using Solution = PaceVC::CutpointsBranching<DoNothingKernel>;

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    Solution(g).branch();
    PaceVC::printSolution(std::cout, g);
    return 0;
}
