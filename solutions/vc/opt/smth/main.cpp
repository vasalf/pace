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

class SolutionStorage {
public:
    SolutionStorage(int expectedMaxSize, const std::vector<int>& aprior)
        : curSolution(aprior)
    {
        curSolution.reserve(expectedMaxSize);
    }

    inline void push(int v) {
        curSolution.push_back(v);
    }

    inline std::size_t size() const {
        return curSolution.size();
    }

    inline void pop() {
        curSolution.pop_back();
    }

    inline void saveTo(std::vector<int>& to) const {
        to.resize(curSolution.size());
        std::memcpy(to.data(), curSolution.data(), sizeof(int) * curSolution.size());
    }

    inline const std::vector<int>& getSolution() const {
        return curSolution;
    }

    inline void swap(SolutionStorage& with) {
        curSolution.swap(with.curSolution);
    }

    inline void append(const std::vector<int>& vertices) {
        int n = curSolution.size();
        curSolution.resize(n + vertices.size());
        std::memcpy(curSolution.data() + n, vertices.data(), sizeof(int) * vertices.size());
    }

    inline bool empty() const {
        return curSolution.empty();
    }

    inline int back() const {
        return curSolution.back();
    }

    inline void shrinkToSize(int sz) {
        curSolution.resize(sz);
    }

private:
    std::vector<int> curSolution;
};

struct ComponentSplitter {
    Graph& graph;

    std::vector<int> order;
    std::vector<int> compSizes;
    std::vector<int> componentOf;

    ComponentSplitter(Graph& g)
        : graph(g)
        , componentOf(g.n)
    {
        order.reserve(g.n);
        compSizes.reserve(g.n);
    }

    void split(const GraphViewRef& graphView) {
        int compId = 0;
        order.clear();
        compSizes.clear();
        for (int u : graphView.leftVertices) {
            componentOf[u] = -1;
        }
        for (int u : graphView.leftVertices) {
            if (componentOf[u] != -1) {
                continue;
            }
            componentOf[u] = compId;
            order.push_back(u);
            int ci = order.size() - 1;
            compSizes.push_back(0);
            while (ci < (int)order.size()) {
                int a = order[ci++];
                compSizes.back()++;
                for (int i = graph.firstEdge[a]; i < graph.lastEdge[a]; i++) {
                    int v = graph.edges[i];
                    if (componentOf[v] == -1) {
                        componentOf[v] = compId;
                        order.push_back(v);
                    }
                }
            }
            compId++;
        }
    }
};

struct LeafHandler {
    Graph& graph;

    int iteration = 0;

    std::vector<int> qbuf;
    std::vector<int> removedOnIteration;
    std::vector<int>& removed;
    SolutionStorage& curSolution;

    LeafHandler(Graph& g, std::vector<int>& removed, SolutionStorage& curSolution)
        : graph(g)
        , removedOnIteration(graph.n)
        , removed(removed)
        , curSolution(curSolution)
    {
        qbuf.reserve(g.n);
    }

    void handle(GraphViewRef& graphView) {
        ++iteration;
        qbuf.clear();
        for (int u : graphView.leftVertices) {
            if (graph.getDegree(u) == 1) {
                qbuf.push_back(u);
            }
        }
        int qhead = 0;
        while (qhead < (int)qbuf.size()) {
            int a = qbuf[qhead++];
            if (graph.getDegree(a) != 1 || removedOnIteration[a] == iteration) {
                continue;
            }
            int b = graph.edges[graph.firstEdge[a]];
            if (removedOnIteration[b] == iteration)
                continue;
            removedOnIteration[a] = iteration;
            removedOnIteration[b] = iteration;
            graph.removeVertex(b);
            graphView.removeVertex(b);
            curSolution.push(b);
            removed.push_back(b);
            for (int i = graph.firstEdge[b]; i < graph.lastEdge[b]; i++) {
                if (graph.getDegree(graph.edges[i]) == 1) {
                    qbuf.push_back(graph.edges[i]);
                }
            }
        }
    }
};

class Brancher {
    Graph& graph;

    ComponentSplitter compSplitter;
    LeafHandler leafHandler;

    std::vector<int> removed;
    std::vector<int> bestSolution;
    SolutionStorage curSolution;

    int curDepth = -1;

#ifdef DEBUG_BRANCHING
    std::string spaces;
#endif

public:
    Brancher(Graph& g, std::vector<int> aprior)
        : graph(g)
        , compSplitter(g)
        , leafHandler(g, removed, curSolution)
        , bestSolution(graph.n)
        , curSolution(g.n, aprior)
    {
        std::iota(bestSolution.begin(), bestSolution.end(), 0);
    }

    void branch() {
        std::vector<int> leftVertices(graph.n);
        std::iota(leftVertices.begin(), leftVertices.end(), 0);
        std::vector<int> which = leftVertices;
        GraphViewRef graphView { leftVertices, which };
        for (int u : curSolution.getSolution()) {
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
    void branchAside(GraphViewRef& graphView) {
        std::vector<int> bestSolutionCopy = graphView.leftVertices;
        SolutionStorage curSolutionCopy(graphView.leftVertices.size(), {});
        bestSolution.swap(bestSolutionCopy);
        curSolution.swap(curSolutionCopy);

        branch(graphView);
        curSolutionCopy.append(bestSolution);

        curSolution.swap(curSolutionCopy);
        bestSolution.swap(bestSolutionCopy);
    }

    void branch(GraphViewRef& graphView) {
        removed.push_back(-1);
        curDepth--;
#ifdef DEBUG_BRANCHING
        spaces.push_back(' ');
#endif
        wrapBranch(graphView);
#ifdef DEBUG_BRANCHING
        spaces.pop_back();
#endif
        curDepth++;
        while (removed.back() != -1) {
            if (!curSolution.empty() && curSolution.back() == removed.back()) {
                curSolution.pop();
            }
            graph.revertVertexRemoval(removed.back());
            graphView.addVertex(removed.back());
            removed.pop_back();
        }
        removed.pop_back();
    }

    void wrapBranch(GraphViewRef& graphView) {
        leafHandler.handle(graphView);

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
#ifdef DEBUG_BRANCHING
            std::cout << spaces << "cutoff(cur=" << curSolution.size() << ",best=" << bestSolution.size() << ")" << std::endl;
#endif
            return;
        }

        if (graphView.leftVertices.empty()) {
            if (bestSolution.size() > curSolution.size()) {
#ifdef DEBUG_BRANCHING
                std::cout << spaces << "solution(size=" << curSolution.size() << ")" << std::endl;
#endif
                curSolution.saveTo(bestSolution);
#ifdef DEBUG_BRANCHING
            } else {
                std::cout << spaces << "solution of same size" << std::endl;
#endif
            }
            return;
        }

        if (curDepth == 0) {
#ifdef DEBUG_BRANCHING
            std::cout << spaces << "leaf(finalSize=" << graphView.leftVertices.size() << ")" << std::endl;
#endif
            return;
        }

        compSplitter.split(graphView);
        if (compSplitter.compSizes.size() > 1) {
            std::vector<int> buffer;
            std::vector<int> compOrder = compSplitter.order;
            std::vector<int> compSizes = compSplitter.compSizes;
            int cstart = 0;
            int rem = curSolution.size();
            for (int sz : compSizes) {
#ifdef DEBUG_BRANCHING
                std::cout << spaces << "component" << std::endl;
#endif
                buffer.resize(sz);
                std::memcpy(buffer.data(), compOrder.data() + cstart, sizeof(int) * sz);
                for (int i = 0; i < sz; i++) {
                    graphView.which[buffer[i]] = i;
                }
                GraphViewRef tmp { buffer, graphView.which };
                branchAside(tmp);
                cstart += sz;
            }
            for (int i = 0; i < (int)graphView.leftVertices.size(); i++) {
                graphView.which[graphView.leftVertices[i]] = i;
            }
            if (curSolution.size() < bestSolution.size()) {
                curSolution.saveTo(bestSolution);
            }
            curSolution.shrinkToSize(rem);
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

#ifdef DEBUG_BRANCHING
            std::cout << spaces << "inner(size=" << graphView.leftVertices.size() << ")" << std::endl;
#endif

        {
#ifdef DEBUG_BRANCHING
            std::cout << spaces << "take(" << v << ")" << std::endl;
#endif
            curSolution.push(v);
            graph.removeVertex(v);
            graphView.removeVertex(v);

            branch(graphView);

            graphView.addVertex(v);
            graph.revertVertexRemoval(v);
            curSolution.pop();
        }

        {
#ifdef DEBUG_BRANCHING
            std::cout << spaces << "takeNeighbours(" << v << ")" << std::endl;
#endif
            int rem = removed.size();
            for (int i = graph.firstEdge[v]; i < graph.lastEdge[v]; i++) {
                removed.push_back(graph.edges[i]);
            }
            for (int i = rem; i < (int)removed.size(); i++) {
                graphView.removeVertex(removed[i]);
                graph.removeVertex(removed[i]);
                curSolution.push(removed[i]);
            }

            branch(graphView);

            for (int i = removed.size() - 1; i >= rem; i--) {
                curSolution.pop();
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
            edges.push_back(Graph::Edge{u, v});
        }
        es++;
    }
    std::sort(aprior.begin(), aprior.end());
    aprior.resize(std::unique(aprior.begin(), aprior.end()) - aprior.begin());
    std::sort(edges.begin(), edges.end());
    edges.resize(std::unique(edges.begin(), edges.end()) - edges.begin());

    g.setEdges(edges);

    Brancher b(g, aprior);
    //b.setMaxDepth(15);
    b.branch();
    auto solution = b.getBestSolution();

    std::cout << "s vc " << n << " " << solution.size() << std::endl;
    for (int u : solution) {
        std::cout << u + 1 << std::endl;
    }

    return 0;
}
