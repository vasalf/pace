#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

struct Graph {
    struct Edge {
        int u, v;

        bool operator<(const Edge& other) const {
            return std::make_pair(u, v) < std::make_pair(other.u, other.v);
        }

        bool operator==(const Edge& other) const {
            return u == other.u && v == other.v;
        }
    };

    int n;
    std::vector<int> firstEdge;
    std::vector<int> lastEdge;
    std::vector<int> edges;
    std::vector<int> revId;

    Graph(int n, int m)
            : n(n)
              , firstEdge(n)
              , lastEdge(n)
              , edges(2 * m)
              , revId(2 * m)
    {}

    Graph(const Graph&) = delete;
    Graph& operator=(const Graph&) = delete;
    Graph(Graph&&) noexcept = default;
    Graph& operator=(Graph&&) noexcept = default;

    void setEdges(std::vector<Edge> setTo) {
        std::vector<int> degree(n);
        for (const auto& edge : setTo) {
            degree[edge.u]++;
            degree[edge.v]++;
        }
        std::partial_sum(degree.begin(), degree.end() - 1, firstEdge.begin() + 1);
        std::memcpy(lastEdge.data(), firstEdge.data(), sizeof(int) * n);
        for (const auto& edge : setTo) {
            revId[lastEdge[edge.u]] = lastEdge[edge.v];
            revId[lastEdge[edge.v]] = lastEdge[edge.u];
            edges[lastEdge[edge.u]++] = edge.v;
            edges[lastEdge[edge.v]++] = edge.u;
        }
    }

    void removeEdge(int u, int id) {
        if (id == lastEdge[u] - 1) {
            lastEdge[u]--;
        } else {
            int lastId = lastEdge[u] - 1;
            int revLastId = revId[lastId];
            revId[revLastId] = id;
            revId[id] = revLastId;
            std::swap(edges[id], edges[lastId]);
            lastEdge[u]--;
        }
    }

    void removeVertex(int v) {
        for (int i = firstEdge[v]; i < lastEdge[v]; i++) {
            assert(edges[i] != v);
            removeEdge(edges[i], revId[i]);
        }
    }

    void revertVertexRemoval(int v) {
        for (int i = firstEdge[v]; i < lastEdge[v]; i++) {
            revId[i] = lastEdge[edges[i]];
            revId[lastEdge[edges[i]]] = i;
            edges[lastEdge[edges[i]]++] = v;
        }
    }

    inline int getDegree(int v) {
        return lastEdge[v] - firstEdge[v];
    }
};

struct GraphViewRef {
    std::vector<int>& leftVertices;
    std::vector<int>& which;

    inline void addVertex(int v) {
        which[v] = leftVertices.size();
        leftVertices.push_back(v);
    }

    inline void removeVertex(int v) {
        if (v == leftVertices.back()) {
            leftVertices.pop_back();
        } else {
            which[leftVertices.back()] = which[v];
            std::swap(leftVertices[which[v]], leftVertices.back());
            leftVertices.pop_back();
        }
    }
};

class Brancher {
    Graph& graph;

    std::vector<int> removed;
    std::vector<int> bestSolution;
    std::vector<int> curSolution;

    int curDepth = -1;

public:
    Brancher(Graph& g, std::vector<int> aprior)
            : graph(g)
              , bestSolution(graph.n)
              , curSolution(aprior)
    {
        std::iota(bestSolution.begin(), bestSolution.end(), 0);
    }

    void branch() {
        std::vector<int> leftVertices(graph.n);
        std::iota(leftVertices.begin(), leftVertices.end(), 0);
        std::vector<int> which = leftVertices;
        GraphViewRef graphView { leftVertices, which };
        for (int u : curSolution) {
            graphView.removeVertex(u);
            graph.removeVertex(u);
        }
        branch(graphView);
    }

    std::vector<int> getBestSolution() {
        return bestSolution;
    }

    void setMaxDepth(int depth) {
        curDepth = depth;
    }

private:
    void branch(GraphViewRef& graphView) {
        removed.push_back(-1);
        curDepth--;
        wrapBranch(graphView);
        curDepth++;
        while (removed.back() != -1) {
            graph.revertVertexRemoval(removed.back());
            graphView.addVertex(removed.back());
            removed.pop_back();
        }
        removed.pop_back();
    }

    void wrapBranch(GraphViewRef& graphView) {
        for (int i = 0; i < (int)graphView.leftVertices.size(); ) {
            if (graph.getDegree(graphView.leftVertices[i]) == 0) {
                removed.push_back(graphView.leftVertices[i]);
                graph.removeVertex(graphView.leftVertices[i]);
                graphView.removeVertex(graphView.leftVertices[i]);
            } else {
                i++;
            }
        }

        if (curSolution.size() > bestSolution.size()) {
            return;
        }

        if (graphView.leftVertices.empty()) {
            if (bestSolution.size() > curSolution.size()) {
                bestSolution.resize(curSolution.size());
                std::memcpy(bestSolution.data(), curSolution.data(), sizeof(int) * curSolution.size());
            }
            return;
        }

        if (curDepth == 0) {
            return;
        }


        int v = graphView.leftVertices[0];
        int mxd = graph.getDegree(v);
        for (int i = 0; i < (int)graphView.leftVertices.size(); i++) {
            int u = graphView.leftVertices[i];
            if (graph.getDegree(u) > mxd) {
                v = u;
                mxd  = graph.getDegree(u);
            }
        }

        {
            curSolution.push_back(v);
            graph.removeVertex(v);
            graphView.removeVertex(v);

            branch(graphView);

            graphView.addVertex(v);
            graph.revertVertexRemoval(v);
            curSolution.pop_back();
        }

        {
            int rem = removed.size();
            for (int i = graph.firstEdge[v]; i < graph.lastEdge[v]; i++) {
                removed.push_back(graph.edges[i]);
            }
            for (int i = rem; i < (int)removed.size(); i++) {
                graphView.removeVertex(removed[i]);
                graph.removeVertex(removed[i]);
                curSolution.push_back(removed[i]);
            }

            branch(graphView);

            for (int i = removed.size() - 1; i >= rem; i--) {
                curSolution.pop_back();
            }
            // State of graph and graphView will be restored in branch() subroutine
        }
    }
};

int main() {
    int n, m;
    while (true) {
        std::string s;
        std::getline(std::cin, s);
        if (s[0] == 'p') {
            std::istringstream ss(s);
            std::string p, td;
            ss >> p >> td >> n >> m;
            break;
        }
    }

    Graph g(n, m);

    std::vector<int> aprior;
    std::vector<Graph::Edge> edges;
    std::string s;
    int es = 0;
    while (es < m && std::getline(std::cin, s)) {
        if (s[0] == 'c') {
            continue;
        }
        std::istringstream ss(s);
        int u, v;
        ss >> u >> v;
        u--; v--;
        if (u == v) {
            aprior.push_back(u);
        } else {
            if (u > v) {
                std::swap(u, v);
            }
            edges.push_back(Graph::Edge {u, v});
        }
        es++;
    }
    std::sort(aprior.begin(), aprior.end());
    aprior.resize(std::unique(aprior.begin(), aprior.end()) - aprior.begin());
    std::sort(edges.begin(), edges.end());
    edges.resize(std::unique(edges.begin(), edges.end()) - edges.begin());

    g.setEdges(edges);

    Brancher b(g, aprior);
    b.setMaxDepth(15);
    b.branch();
    auto solution = b.getBestSolution();

    std::cout << "s vc " << n << " " << solution.size() << std::endl;
    for (int u : solution) {
        std::cout << u + 1 << std::endl;
    }

    return 0;
}
