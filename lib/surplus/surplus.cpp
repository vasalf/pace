#include <surplus/surplus.h>

#include <bipartite_maxm/best.h>
#include <graph/bipartite_graph.h>

namespace {

PaceVC::BipartiteGraph duplicateVertices(const PaceVC::Graph& graph, int skipVertex = -1) {
    PaceVC::BipartiteGraph ret(graph.realSize(), graph.realSize());

    for (int u : graph.undecided()) {
        if (u == skipVertex)
            continue;
        if (graph.adjacent(u).count(skipVertex))
            continue;
        for (int v : graph.adjacent(u)) {
            if (graph.adjacent(v).count(skipVertex))
                continue;
            ret.addEdge(u, v);
        }
    }

    return ret;
}

}

namespace PaceVC {

MinimalSurplusSetFinder::MinimalSurplusSetFinder(const Graph& graph)
    : g(graph)
{}

int MinimalSurplusSetFinder::find() {
    BipartiteGraph h = duplicateVertices(g);

    BestMinVCFinder vc(h);
    vc.find();
    std::vector<int> count(h.leftSize());
    for (auto p : vc.answer) {
        count[p.second]++;
    }

    for (int i = 0; i < g.realSize(); i++) {
        if (count[i] == 0)
            set.push_back(i);
        else if (count[i] == 2)
            neighbours.push_back(i);
    }

    return (int)neighbours.size() - (int)set.size();
}

MinimalSurplusNonEmptySetFinder::MinimalSurplusNonEmptySetFinder(const Graph& graph)
    : g(graph)
{}

int MinimalSurplusNonEmptySetFinder::find() {
    MinimalSurplusSetFinder trial(g);
    if (trial.set.size() > 0) {
        set = trial.set;
        neighbours = trial.neighbours;
        return (int)neighbours.size() - (int)set.size();
    }

    int ret = g.size();

    for (int u : g.undecided()) {
        BipartiteGraph h = duplicateVertices(g, u);

        BestMinVCFinder vc(h);
        vc.find();

        std::vector<int> count(h.leftSize());
        for (auto p : vc.answer) {
            count[p.second]++;
        }

        std::vector<int> cset, cneighbours;

        for (int i = 0; i < g.realSize(); i++) {
            if (count[i] == 0 && !g.adjacent(u).count(i))
                cset.push_back(i);
            else if (count[i] == 2)
                cneighbours.push_back(i);
        }

        if (count[u] != 0)
            set.push_back(u);
        for (int v : g.adjacent(u))
            if (count[v] != 2)
                cneighbours.push_back(v);

        if ((int)cneighbours.size() - (int)cset.size() < ret) {
            set = cset;
            neighbours = cneighbours;
            ret = (int)neighbours.size() - set.size();
        }
    }

    return ret;
}

}
