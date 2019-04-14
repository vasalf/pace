#include <kernels/lp/lp.h>

#include <graph/bipartite_graph.h>
#include <bipartite_maxm/best.h>
#include <kernels/common.h>
#include <surplus/surplus.h>

#include <algorithm>

namespace {

template<class SurplusFinder>
void reduceImpl(PaceVC::Graph& graph, const SurplusFinder& surplus) {
    for (int u : surplus.set)
        graph.removeVertex(u);
    for (int v : surplus.neighbours)
        graph.takeVertex(v);
}

}

namespace PaceVC {
namespace Kernels {

LPKernel::LPKernel(Graph& g)
    : graph(g)
{}

void LPKernel::reduce() {
    MinimalSurplusSetFinder surplus(graph);
    int val = surplus.find();

    if (val > 0) {
        bound = graph.size() - val;
        lowerBound = (graph.size() + 1) / 2;
        return;
    }

    bound = -1;
    reduceImpl(graph, surplus);

    lowerBound = (graph.size() + 1) / 2;
}

ZeroSurplusLPKernel::ZeroSurplusLPKernel(Graph& g)
    : graph(g)
{}

void ZeroSurplusLPKernel::reduce() {
    MinimalSurplusSetFinder surplus(graph);
    int val = surplus.find();

    if (val > 0) {
        bound = graph.size() - val;
        lowerBound = (graph.size() + 1) / 2;
        return;
    }

    bound = -1;

    if (val < 0) {
        reduceImpl(graph, surplus);
        lowerBound = graph.size() / 2;
        return;
    }

    auto undecidedCopy = graph.undecided();
    for (int u : undecidedCopy) {
        if (!graph.undecided().count(u))
            continue;

        Graph copy = graph;
        for (int v : graph.adjacent(u))
            copy.takeVertex(v);
        copy.removeVertex(u);
        MinimalSurplusSetFinder surplusCopy(copy);
        int copyVal = surplusCopy.find();

        if (copyVal + graph.adjacent(u).size() - 1 == 0) {
            auto adjCopy = graph.adjacent(u);
            for (int v : adjCopy)
                graph.takeVertex(v);
            graph.removeVertex(u);
            reduceImpl(graph, surplusCopy);
        }
    }

    lowerBound = (graph.size() + 1) / 2;
}

FastZeroSurplusLPKernel::FastZeroSurplusLPKernel(Graph& g)
    : graph(g)
{}

namespace {
    BipartiteGraph duplicateVertices(const Graph& graph) {
        BipartiteGraph ret(graph.realSize(), graph.realSize());
        for (int u : graph.undecided()) {
            for (int v : graph.adjacent(u))
                ret.addEdge(u, v);
        }
        return ret;
    }

    class DirectedGraph {
    public:
        DirectedGraph()
            : DirectedGraph(0)
        {}

        DirectedGraph(int size)
            : edges_(size)
        {}

        void addEdge(int u, int v) {
            edges_[u].push_back(v);
        }

        const std::vector<int>& adjacent(int u) const {
            return edges_[u];
        }

        int size() const {
            return edges_.size();
        }

    private:
        std::vector<std::vector<int>> edges_;
    };

    DirectedGraph reversedEdges(const DirectedGraph& of) {
        DirectedGraph ret(of.size());

        for (int i = 0; i < of.size(); i++) {
            for (int u : of.adjacent(i))
                ret.addEdge(u, i);
        }

        return ret;
    }

    struct StronglyConnectedComponentsFinder {
        DirectedGraph& graph;

        std::vector<int> topsort;
        std::vector<bool> vis;

        void topsortDfs(int u) {
            vis[u] = true;
            for (int v : graph.adjacent(u)) {
                if (!vis[v])
                    topsortDfs(v);
            }
            topsort.push_back(u);
        }

        DirectedGraph rev;
        std::vector<int> compId;

        void revMarkDfs(int u, int mark) {
            compId[u] = mark;
            for (int v : rev.adjacent(u)) {
                if (compId[v] == -1) {
                    revMarkDfs(v, mark);
                }
            }
        }

        StronglyConnectedComponentsFinder(DirectedGraph& g)
            : graph(g)
        {}

        void find() {
            vis.resize(graph.size(), false);
            topsort.reserve(graph.size());
            for (int i = 0; i < graph.size(); i++) {
                if (!vis[i])
                    topsortDfs(i);
            }
            std::reverse(topsort.begin(), topsort.end());
            rev = reversedEdges(graph);
            compId.resize(graph.size(), -1);
            int comp = 0;
            for (int u : topsort) {
                if (compId[u] == -1)
                    revMarkDfs(u, comp++);
            }
        }
    };
}

void FastZeroSurplusLPKernel::reduce() {
    LPKernel(graph).reduce();

    auto bigraph = duplicateVertices(graph);
    BestMatchingFinder maxm(bigraph);
    int sz = maxm.find();
    assert(sz == graph.size());

    DirectedGraph implicationGraph(graph.realSize());
    for (int u : graph.undecided()) {
        for (int v : graph.adjacent(u)) {
            implicationGraph.addEdge(u, maxm.pair[v]);
        }
    }

    StronglyConnectedComponentsFinder scc(implicationGraph);
    scc.find();

    auto undecidedCopy = graph.undecided();
    for (int u : undecidedCopy) {
        int a = scc.compId[u];
        int b = scc.compId[maxm.pairRight[u]];
        if (a < b) {
            graph.takeVertex(u);
        } else if (b < a) {
            graph.removeVertex(u);
        }
    }
}

}
}
