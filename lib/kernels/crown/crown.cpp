#include <kernels/crown/crown.h>

#include <bipartite_maxm/best.h>
#include <graph/bipartite_graph.h>
#include <kernels/common.h>

namespace {

std::vector<bool> maximalMatching(PaceVC::Graph& g) {
    std::vector<bool> ret(g.realSize());

    for (int i : g.undecided()) {
        for (int u : g.adjacent(i))
            if (!ret[i] && !ret[u])
                ret[i] = ret[u] = true;
    }

    return ret;
}

}

namespace PaceVC {
namespace Kernels {

CrownKernel::CrownKernel(Graph& g)
    : graph(g)
{}

void CrownKernel::reduce() {
    std::vector<bool> covered = maximalMatching(graph);

    std::vector<int> idOfLeft, idOfRight;
    std::vector<int> idInPart(graph.realSize());
    for (int i : graph.undecided()) {
        if (covered[i]) {
            idInPart[i] = idOfLeft.size();
            idOfLeft.push_back(i);
        } else {
            idInPart[i] = idOfRight.size();
            idOfRight.push_back(i);
        }
    }

    BipartiteGraph bg(idOfLeft.size(), idOfRight.size());
    for (int u : idOfRight)
        for (int v : graph.adjacent(u)) {
            // Right part is an independent set so we don't need to verify
            // that v lies in the left part
            bg.addEdge(idInPart[v], idInPart[u]);
        }

    BestMinVCFinder vc(bg);
    vc.find();

    std::vector<bool> inVC(graph.realSize());
    for (auto p : vc.answer) {
        if (p.first == BipartiteGraph::Part::LEFT)
            inVC[idOfLeft[p.second]] = true;
        else
            inVC[idOfRight[p.second]] = true;
    }

    for (int u : idOfLeft) {
        if (inVC[u])
            graph.takeVertex(u);
    }

    for (int i = 0; i < bg.rightSize(); i++)
        if (vc.maxm.pair[i] != -1 && inVC[idOfLeft[vc.maxm.pair[i]]])
            graph.removeVertex(idOfRight[i]);

    cleanUp(graph);
}

}
}
