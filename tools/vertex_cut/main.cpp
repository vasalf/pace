#include <cut/vertex_cut.h>
#include <graph/graph.h>
#include <graph/util.h>
#include <kernels/trivial.h>

#include <iostream>

int main() {
    PaceVC::Graph g = PaceVC::readGraph(std::cin);
    for (int t = 0; t < 5; t++) {
        PaceVC::Graph graph = g;
        PaceVC::Kernels::Trivial(graph).reduce();
        std::vector<int> cut;
        if (!PaceVC::cutpoints(graph).empty()) {
            std::cout << 1 << std::endl;
            cut = {PaceVC::cutpoints(graph)[0]};
        } else {
            PaceVC::VertexCutFinder vcf(graph);
            std::cout << vcf.find() << std::endl;
            cut = vcf.answer;
        }
        for (int u : cut) {
            graph.removeVertex(u);
        }
        for (auto v : PaceVC::connectedComponents(graph)) {
            std::cout << v.size() << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}
