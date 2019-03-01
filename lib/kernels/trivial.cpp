#include <kernels/trivial.h>

#include <queue>

namespace PaceVC {
namespace Kernels {
namespace {
    void handleLeaves(Graph& graph) {
        Graph::Set<int> undecidedCopy = graph.undecided();

        std::queue<int> leaves;
        for (int u : undecidedCopy) {
            if (graph.adjacent(u).size() == 1)
                leaves.push(u);
        }

        while (!leaves.empty()) {
            int a = leaves.front();
            leaves.pop();

            if (!graph.undecided().count(a))
                continue;

            int u = -1;
            if (graph.adjacent(a).size() == 1) {
                u = *(graph.adjacent(a).begin());
            }
            graph.removeVertex(a);

            if (u != -1) {
                auto adjCopy = graph.adjacent(u);
                graph.takeVertex(u);
                for (int v : adjCopy) {
                    if (graph.adjacent(v).size() == 1) {
                        leaves.push(v);
                    }
                }
            }
        }
    }

    struct Degree2Handler {
        Graph& graph;

        std::vector<bool> used;
        std::vector<int> cpath;

        void dfs(int v) {
            used[v] = true;
            cpath.push_back(v);
            for (int u : graph.adjacent(v)) {
                if (!used[u] && graph.adjacent(u).size() == 2) {
                    dfs(u);
                }
            }
        }

        bool isStartVertex(int v) {
            if (graph.adjacent(v).size() != 2)
                return false;
            int a = *(graph.adjacent(v).begin());
            int b = *std::next(graph.adjacent(v).begin());
            return graph.adjacent(a).size() != 2 || graph.adjacent(b).size() != 2;
        }

        int neededNeighbour(int v) {
            int a = *(graph.adjacent(v).begin());
            int b = *std::next(graph.adjacent(v).begin());
            return graph.adjacent(a).size() != 2 ? a : b;
        }

        void splitVector(const std::vector<int>& toSplit, std::vector<int>& even, std::vector<int>& odd) {
            for (int i = 0; i < (int)toSplit.size(); i++) {
                if (i % 2 == 0) {
                    even.push_back(toSplit[i]);
                } else {
                    odd.push_back(toSplit[i]);
                }
            }
        }

        Degree2Handler(Graph& g)
            : graph(g)
        {}

        void handle() {
            used.resize(graph.realSize(), false);

            std::vector<std::vector<int>> paths;
            for (int u : graph.undecided()) {
                if (!used[u] && isStartVertex(u)) {
                    cpath.clear();
                    dfs(u);
                    paths.push_back(cpath);
                }
            }

            std::vector<std::vector<int>> cycles;
            for (int u : graph.undecided()) {
                if (!used[u] && graph.adjacent(u).size() == 2) {
                    cpath.clear();
                    dfs(u);
                    cycles.push_back(cpath);
                }
            }

            for (const auto& p : paths) {
                std::vector<int> a, b;
                if (p.size() % 2 == 1) {
                    splitVector(p, a, b);
                } else {
                    splitVector(p, b, a);
                    b.push_back(neededNeighbour(p[0]));
                }
                graph.span(p, a, b);
            }

            for (const auto& p : cycles) {
                for (int i = 0; i < (int)p.size(); i++) {
                    if (i % 2 == 0)
                        graph.takeVertex(p[i]);
                    else
                        graph.removeVertex(p[i]);
                }
            }
        }
    };

    void handleIsolated(Graph& graph) {
        Graph::Set<int> undecidedCopy = graph.undecided();
        for (int u : undecidedCopy) {
            if (graph.adjacent(u).empty()) {
                graph.removeVertex(u);
            }
        }
    }
}

TrivialImpl::TrivialImpl(Graph& g)
    : graph(g)
{}

void TrivialImpl::reduce() {
    handleLeaves(graph);
    Degree2Handler(graph).handle();
    handleIsolated(graph);
}

}
}
