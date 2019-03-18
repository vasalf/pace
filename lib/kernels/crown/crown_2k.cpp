#include <kernels/crown/crown_2k.h>
#include <kernels/common.h>

#include <graph/boost_graph.h>

#include <boost/graph/max_cardinality_matching.hpp>

namespace {
    struct Matching {
        std::vector<int> pair;

        Matching() = default;

        Matching(int n)
            : pair(n, -1)
        { }

        void removeEdge(int u) {
            int v = pair[u];
            pair[v] = pair[u] = -1;
        }

        void forceAddEdge(int u, int v) {
            if (pair[u] != -1)
                removeEdge(u);
            if (pair[v] != -1)
                removeEdge(v);
            addEdge(u, v);
        }

        void addEdge(int u, int v) {
            assert(pair[u] == -1);
            assert(pair[v] == -1);
            pair[u] = v;
            pair[v] = u;
        }

        inline bool isCovered(int v) {
            return pair[v] != -1;
        }

        void switchThroughPath(const std::vector<int>& path) {
            assert(path.size() % 2 == 1);
            for (int i = 0; i + 1 < (int)path.size(); i += 2) {
                assert(!isCovered(path[i]));
                assert(isCovered(path[i + 1]));
                forceAddEdge(path[i], path[i + 1]);
            }
        }
    };

    Matching maxMatching(const PaceVC::Graph& g) {
        PaceVC::Boost::Graph boostGraph = PaceVC::toBoostGraph(g);

        std::vector<PaceVC::Boost::Vertex> mate(g.realSize());
        boost::edmonds_maximum_cardinality_matching(boostGraph, mate.data());

        Matching m(g.realSize());

        PaceVC::Boost::VertexIterator vi, vi_end;
        for (std::tie(vi, vi_end) = boost::vertices(boostGraph); vi != vi_end; vi++) {
            if (mate[*vi] != PaceVC::Boost::GraphTraits::null_vertex()) {
                m.pair[*vi] = mate[*vi];
            }
        }

        return m;
    }

    std::pair<int, int> findEdgeInSet(const PaceVC::Graph& g, std::vector<int> set) {
        std::unordered_set<int> st;
        for (int u : set) {
            st.insert(u);
        }

        for (int v : set) {
            for (int u : g.adjacent(v)) {
                if (st.count(u)) {
                    return std::make_pair(u, v);
                }
            }
        }

        return std::make_pair(-1, -1);
    }
}

namespace PaceVC {
namespace Kernels {

struct Crown2kKernel::TImpl {
    Graph& graph;
    int lowerBound = 0;

    TImpl(Graph& g)
        : graph(g)
    {}

    Matching m;
    std::vector<bool> isInOddCycle;

    std::vector<bool> isInHead;
    std::vector<bool> isInCrown;
    std::vector<int> prev;

    void handleOddCycle(int v, int ui, int wi) {
        std::vector<int> a;
        std::vector<int> b;
        while (ui != wi) {
            a.push_back(ui);
            b.push_back(wi);
            isInOddCycle[ui] = true;
            isInOddCycle[wi] = true;
            ui = prev[ui];
            wi = prev[wi];
        }
        isInOddCycle[ui] = true;
        a.push_back(ui);
        while (ui != v) {
            ui = prev[ui];
            a.push_back(ui);
        }

        std::vector<int> path;
        path.reserve(a.size() + b.size());
        std::copy(a.rbegin(), a.rend(), std::back_inserter(path));
        std::copy(b.begin(), b.end(), std::back_inserter(path));
        m.switchThroughPath(path);
    }

    void reduce() {
        m = maxMatching(graph);
        isInOddCycle.assign(graph.realSize(), false);
        while (true) {
            for (int u : graph.undecided()) {
                bool ok = false;
                for (int v : graph.adjacent(u)) {
                    if (!isInOddCycle[v]) {
                        ok = true;
                        break;
                    }
                }
                if (!ok) {
                    isInOddCycle[u] = true;
                }
            }

            int v = -1;
            for (int u : graph.undecided()) {
                if (!m.isCovered(u) && !isInOddCycle[u]) {
                    v = u;
                    break;
                }
            }
            if (v == -1) {
                break;
            }

            isInHead.assign(graph.realSize(), false);
            isInCrown.assign(graph.realSize(), false);
            prev.assign(graph.realSize(), -1);

            isInCrown[v] = true;
            std::vector<int> lastHead;
            for (int u : graph.adjacent(v)) {
                assert(!isInOddCycle[u]);
                isInHead[u] = true;
                lastHead.push_back(u);
                prev[u] = v;
            }

            bool foundCrown = true;

            while (!lastHead.empty()) {
                int ui = -1, wi = -1;
                for (int u : lastHead) {
                    assert(m.isCovered(u));
                    if (isInHead[m.pair[u]]) {
                        ui = u;
                        wi = m.pair[u];
                        break;
                    }
                }
                if (ui != -1) {
                    handleOddCycle(v, ui, wi);
                    foundCrown = false;
                    break;
                }

                std::vector<int> lastCrown;
                for (int u : lastHead) {
                    int w = m.pair[u];
                    lastCrown.push_back(w);
                    isInCrown[w] = true;
                    prev[w] = u;
                }
                std::tie(ui, wi) = findEdgeInSet(graph, lastCrown);
                if (ui != -1) {
                    handleOddCycle(v, ui, wi);
                    foundCrown = false;
                    break;
                }

                lastHead.clear();
                for (int u : lastCrown) {
                    for (int w : graph.adjacent(u)) {
                        if (isInHead[w] || isInCrown[w]) {
                            continue;
                        }
                        assert(!isInOddCycle[w]);
                        lastHead.push_back(w);
                        isInHead[w] = true;
                        prev[w] = u;
                    }
                }
            }

            if (foundCrown) {
                auto undecidedCopy = graph.undecided();
                for (int u : undecidedCopy) {
                    if (isInCrown[u]) {
                        graph.removeVertex(u);
                    } else if (isInHead[u]) {
                        graph.takeVertex(u);
                    }
                }
                undecidedCopy = graph.undecided();
                for (int u : undecidedCopy) {
                    if (graph.adjacent(u).empty()) {
                        graph.removeVertex(u);
                    }
                }
                return;
            }
        }
        lowerBound = (graph.size() + 1) / 2;
        cleanUp(graph);
    }
};

Crown2kKernel::Crown2kKernel(Graph& g)
    : impl_(std::make_unique<TImpl>(g))
{}

Crown2kKernel::~Crown2kKernel() = default;

void Crown2kKernel::reduce() {
    impl_->reduce();
    lowerBound = impl_->lowerBound;
}

}
}
