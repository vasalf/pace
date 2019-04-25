#include <cut/vertex_cut.h>
#include <graph/graph.h>
#include <graph/util.h>
#include <kernels/trivial.h>

#include <iostream>

int main() {
    PaceVC::Graph graph = PaceVC::readGraph(std::cin);
    PaceVC::Kernels::Trivial(graph).reduce();
    if (!PaceVC::cutpoints(graph).empty()) {
        std::cout << 1 << std::endl;
    } else {
        PaceVC::VertexCutFinder vcf(graph);
        std::cout << vcf.find() << std::endl;
    }
    return 0;
}
