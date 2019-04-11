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

    struct EdgePosInfo {
        int uid, vid;

        inline int& that(int id) {
            assert(uid == id || vid == id);
            return id == uid ? uid : vid;
        }

        inline int& other(int id) {
            assert(uid == id || vid == id);
            return id == uid ? vid : uid;
        }
    };

    int n;
    std::vector<int> firstEdge;
    std::vector<int> lastEdge;
    std::vector<int> edges;
    std::vector<int> id;
    std::vector<EdgePosInfo> edgeInfos;

    Graph(int n, int m)
            : n(n)
              , firstEdge(n)
              , lastEdge(n)
              , edges(2 * m)
              , id(2 * m)
    {
        firstEdge.reserve(2 * n);
        lastEdge.reserve(2 * n);
        edgeInfos.reserve(m);
    }

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
            edgeInfos.push_back({lastEdge[edge.v], lastEdge[edge.u]});
            id[lastEdge[edge.u]] = edgeInfos.size() - 1;
            id[lastEdge[edge.v]] = edgeInfos.size() - 1;
            edges[lastEdge[edge.u]++] = edge.v;
            edges[lastEdge[edge.v]++] = edge.u;
        }
    }

    void removeEdge(int u, int eid) {
        if (eid == lastEdge[u] - 1) {
            lastEdge[u]--;
        } else {
            int lastId = lastEdge[u] - 1;
            edgeInfos[id[eid]].that(eid) = lastId;
            edgeInfos[id[lastId]].that(lastId) = eid;
            std::swap(edges[eid], edges[lastId]);
            std::swap(id[eid], id[lastId]);
            lastEdge[u]--;
        }
    }

    void removeVertex(int v) {
        for (int i = firstEdge[v]; i < lastEdge[v]; i++) {
            assert(edges[i] != v);
            removeEdge(edges[i], edgeInfos[id[i]].other(i));
        }
    }

    void revertVertexRemoval(int v) {
        for (int i = firstEdge[v]; i < lastEdge[v]; i++) {
            edgeInfos[id[i]].other(i) = lastEdge[edges[i]];
            id[lastEdge[edges[i]]] = id[i];
            edges[lastEdge[edges[i]]++] = v;
        }
    }

    inline int getDegree(int v) {
        return lastEdge[v] - firstEdge[v];
    }

    int addFakeVertex() {
        firstEdge.push_back(edges.size());
        lastEdge.push_back(edges.size());
        return n++;
    }

    void delegateVertexToFake(int v, int igu) {
        int last = n - 1;
        for (int i = firstEdge[v]; i < lastEdge[v]; i++) {
            if (edges[i] == igu)
                continue;
            edges.push_back(edges[i]);
            id.push_back(id[i]);
            edges[edgeInfos[id[i]].other(i)] = last;
            edgeInfos[id[i]].that(i) = edges.size() - 1;
            lastEdge[last]++;
        }
    }

    void revertVertexDelegation(int v) {
        for (int i = firstEdge[v]; i < lastEdge[v]; i++) {
            int u = edges[i];
            if (firstEdge[u] <= edgeInfos[id[i]].uid && edgeInfos[id[i]].uid < lastEdge[u]) {
                if (edges[edgeInfos[id[i]].uid] != n - 1) {
                    continue;
                }
                edgeInfos[id[i]].vid = i;
                edges[edgeInfos[id[i]].uid] = v;
            } else {
                if (edges[edgeInfos[id[i]].vid] != n - 1) {
                    continue;
                }
                edgeInfos[id[i]].uid = i;
                edges[edgeInfos[id[i]].vid] = v;
            }
        }
    }

    void removeFakeVertex() {
        int last = n - 1;
        edges.resize(firstEdge[last]);
        id.resize(firstEdge[last]);
        firstEdge.pop_back();
        lastEdge.pop_back();
        n--;
    }

    bool areAdjacent(int u, int v) {
        if (getDegree(u) > getDegree(v))
            std::swap(u, v);
        for (int i = firstEdge[u]; i < lastEdge[u]; i++) {
            if (edges[i] == v)
                return true;
        }
        return false;
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

    inline void addFakeVertex() {
        which.push_back(leftVertices.size());
        leftVertices.push_back(which.size() - 1);
    }

    inline void removeFakeVertex() {
        removeVertex(which.size() - 1);
        which.pop_back();
    }

    inline bool isIn(int v) {
        return 0 <= which[v] && which[v] < (int)leftVertices.size() && leftVertices[which[v]] == v;
    }
};

class SolutionStorage {
public:
    SolutionStorage(int n, const std::vector<int>& aprior)
        : status(n, VertexStatus::FREE)
        , delegationList(n)
        , delegationListStart(n + 1)
        , curSolution(aprior)
    {
        qbuf.reserve(2 * n);
        status.reserve(2 * n);
        delegationList.reserve(2 * n);
        std::iota(delegationList.begin(), delegationList.end(), 0);
        std::iota(delegationListStart.begin(), delegationListStart.end(), 0);
        delegationListStart.reserve(2 * n + 1);
        curSolution.reserve(n);
        for (int u : curSolution) {
            status[u] = VertexStatus::TOOK;
        }
    }

    void push(int v) {
        curSolution.push_back(v);
        status[v] = VertexStatus::TOOK;
    }

    std::size_t size() const {
        return curSolution.size() + above;
    }

    void pop() {
        status[curSolution.back()] = VertexStatus::FREE;
        curSolution.pop_back();
    }

    void saveTo(std::vector<int>& to) const {
        to.clear();
        to.reserve(size());
        std::vector<VertexStatus> statusCopy = status;
        for (int i = status.size() - 1; i >= 0; i--) {
            assert(statusCopy[i] != VertexStatus::DELEGATED);
            if (statusCopy[i] == VertexStatus::IGNORED)
                continue;
            for (int j = delegationListStart[i]; j < delegationListStart[i + 1]; j++) {
                if ((statusCopy[i] == VertexStatus::TOOK && (j - delegationListStart[i]) % 2 == 0)
                        || (statusCopy[i] == VertexStatus::FREE && (j - delegationListStart[i]) % 2 == 1)) {
                    if (delegationList[j] == i) {
                        to.push_back(i);
                    } else {
                        statusCopy[delegationList[j]] = VertexStatus::TOOK;
                    }
                } else {
                    statusCopy[delegationList[j]] = VertexStatus::FREE;
                }
            }
        }
    }

    SolutionStorage makeEmptyCopy(GraphViewRef& graphView) const {
        SolutionStorage copy = *this;
        copy.curSolution.clear();
        copy.above = 0;
        for (auto& s : copy.status)
            s = VertexStatus::IGNORED;
        for (int u : graphView.leftVertices) {
            copy.status[u] = VertexStatus::FREE;
            qbuf.clear();
            qbuf.push_back(u);
            int qit = 0;
            while (qit < (int)qbuf.size()) {
                int a = qbuf[qit++];
                copy.above += (delegationListStart[a + 1] - delegationListStart[a]) / 2;
                for (int i = delegationListStart[a]; i < delegationListStart[a + 1]; i++) {
                    if (delegationList[i] != a) {
                        qbuf.push_back(delegationList[i]);
                    }
                }
            }
        }
        return copy;
    }

    std::vector<int> getSolution() const {
        std::vector<int> ret;
        saveTo(ret);
        return ret;
    }

    void swap(SolutionStorage& with) {
        std::swap(above, with.above);
        status.swap(with.status);
        delegationList.swap(with.delegationList);
        delegationListStart.swap(with.delegationListStart);
        curSolution.swap(with.curSolution);
    }

    void append(const std::vector<int>& v) {
        curSolution.resize(curSolution.size() + v.size());
        std::memcpy(curSolution.data() + curSolution.size() - v.size(), v.data(), sizeof(int) * v.size());
        for (int u : v) {
            status[u] = VertexStatus::TOOK;
        }
    }

    void shrinkToSize(int sz) {
        sz -= above;
        for (int i = sz; i < (int)curSolution.size(); i++) {
            status[curSolution[i]] = VertexStatus::FREE;
        }
        curSolution.resize(sz);
    }

    bool empty() const {
        return curSolution.empty();
    }

    int back() const {
        return curSolution.back();
    }

    inline void addFakeVertex() {
        status.push_back(VertexStatus::FREE);
        delegationListStart.push_back(delegationList.size());
    }

    inline void delegateVertexToFake(int v) {
        assert(status[v] == VertexStatus::FREE);
        delegationList.push_back(v);
        delegationListStart.back()++;
        if ((delegationListStart.back() - delegationListStart[delegationListStart.size() - 2]) % 2 == 0) {
            above++;
        }
        status[v] = VertexStatus::DELEGATED;
    }

    inline void removeFakeVertex(GraphViewRef& graphView) {
        status.pop_back();
        for (int i = delegationListStart[delegationListStart.size() - 2]; i < (int)delegationList.size(); i++) {
            if ((i - delegationListStart[delegationListStart.size() - 2]) % 2 == 1) {
                above--;
                assert(above >= 0);
            }
            status[delegationList[i]] = VertexStatus::FREE;
            graphView.addVertex(delegationList[i]);
        }
        delegationList.resize(delegationListStart[delegationListStart.size() - 2]);
        delegationListStart.pop_back();
    }

    inline std::vector<int> ifTookAll(GraphViewRef& graphView) {
        append(graphView.leftVertices);
        std::vector<int> ret = getSolution();
        shrinkToSize(above);
        return ret;
    }

private:
    enum class VertexStatus {
        FREE,
        TOOK,
        DELEGATED,
        IGNORED
    };

    int above = 0;
    std::vector<VertexStatus> status;
    std::vector<int> delegationList;
    std::vector<int> delegationListStart;
    std::vector<int> curSolution;
    mutable std::vector<int> qbuf;
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
        order.reserve(2 * g.n);
        compSizes.reserve(2 * g.n);
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

    void addFakeVertex() {
        componentOf.push_back(0);
    }

    void removeFakeVertex() {
        componentOf.pop_back();
    }
};

struct LeafHandler {
    Graph& graph;

    int iteration = 0;

    std::vector<int> qbuf;
    std::vector<int> removedAtIteration;
    std::vector<int>& removed;
    SolutionStorage& curSolution;

    LeafHandler(Graph& g, std::vector<int>& removed, SolutionStorage& curSolution)
        : graph(g)
        , removedAtIteration(graph.n)
        , removed(removed)
        , curSolution(curSolution)
    {
        removedAtIteration.reserve(2 * graph.n);
        qbuf.reserve(2 * g.n);
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
            if (graph.getDegree(a) != 1 || removedAtIteration[a] == iteration) {
                continue;
            }
            int b = graph.edges[graph.firstEdge[a]];
            if (removedAtIteration[b] == iteration)
                continue;
            removedAtIteration[a] = iteration;
            removedAtIteration[b] = iteration;
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

    void addFakeVertex() {
        removedAtIteration.push_back(0);
    }

    void removeFakeVertex() {
        removedAtIteration.pop_back();
    }
};

struct PassageHandler {
    Graph& graph;
    ComponentSplitter& componentSplitter;
    LeafHandler& leafHandler;
    SolutionStorage& solutionStorage;
    std::vector<int>& removed;

    std::vector<int> leftVBuf;

    PassageHandler(Graph& graph,
                   ComponentSplitter& cs,
                   LeafHandler& lh,
                   SolutionStorage& ss,
                   std::vector<int>& removed)
        : graph(graph)
        , componentSplitter(cs)
        , leafHandler(lh)
        , solutionStorage(ss)
        , removed(removed)
    {
        leftVBuf.reserve(graph.n);
    }

    inline bool isStartVertex(int v) {
        return graph.getDegree(v) == 2
            && (graph.getDegree(graph.edges[graph.firstEdge[v]]) > 2
                || graph.getDegree(graph.edges[graph.firstEdge[v] + 1]) > 2);
    }

    inline int otherNeighbour(int v, int notThat) {
        return graph.edges[graph.firstEdge[v]] == notThat
            ? graph.edges[graph.firstEdge[v] + 1]
            : graph.edges[graph.firstEdge[v]];
    }

    void handle(GraphViewRef& graphView) {
        leftVBuf.resize(graphView.leftVertices.size());
        std::memcpy(leftVBuf.data(), graphView.leftVertices.data(), sizeof(int) * graphView.leftVertices.size());
        for (int u : leftVBuf) {
            if (graphView.isIn(u) && isStartVertex(u)) {
                int vs = graph.getDegree(graph.edges[graph.firstEdge[u]]) > 2
                       ? graph.edges[graph.firstEdge[u] + 1]
                       : graph.edges[graph.firstEdge[u]];
                int v = vs;
                int vp = u;
                int w = u;
                u = otherNeighbour(vp, v);
                while (graph.getDegree(v) == 2 && graph.getDegree(otherNeighbour(v, vp)) == 2) {
                    vp = otherNeighbour(v, vp);
                    v = otherNeighbour(vp, v);
                }
                if (v == u || graph.areAdjacent(u, v)) {
                    solutionStorage.push(v);
                    removed.push_back(v);
                    graph.removeVertex(v);
                    graphView.removeVertex(v);
                    if (v != u) {
                        solutionStorage.push(u);
                        removed.push_back(u);
                        graph.removeVertex(u);
                        graphView.removeVertex(u);
                    }
                    continue;
                }
                // graphView && solutionStorage
                graphView.addFakeVertex();
                solutionStorage.addFakeVertex();
                vp = w;
                graphView.removeVertex(u);
                solutionStorage.delegateVertexToFake(u);
                while (graph.getDegree(vs) == 2 && graph.getDegree(otherNeighbour(vs, vp)) == 2) {
                    graphView.removeVertex(vp);
                    solutionStorage.delegateVertexToFake(vp);
                    graphView.removeVertex(vs);
                    solutionStorage.delegateVertexToFake(vs);
                    vp = otherNeighbour(vs, vp);
                    vs = otherNeighbour(vp, vs);
                }
                graphView.removeVertex(vp);
                solutionStorage.delegateVertexToFake(vp);
                graphView.removeVertex(v);
                solutionStorage.delegateVertexToFake(v);
                // others
                graph.addFakeVertex();
                graph.delegateVertexToFake(u, w);
                graph.delegateVertexToFake(v, vp);
                removed.push_back(-2);
                removed.push_back(u);
                removed.push_back(v);
                removed.push_back(-2);
                componentSplitter.addFakeVertex();
                leafHandler.addFakeVertex();
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

    PassageHandler passageHandler;

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
        , passageHandler(graph, compSplitter, leafHandler, curSolution, removed)
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
        SolutionStorage curSolutionCopy = curSolution.makeEmptyCopy(graphView);
        std::vector<int> bestSolutionCopy = curSolutionCopy.ifTookAll(graphView);
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
            if (removed.back() == -2) {
                removed.pop_back();
                while (removed.back() != -2) {
                    graph.revertVertexDelegation(removed.back());
                    removed.pop_back();
                }
                removed.pop_back();
                graph.removeFakeVertex();
                graphView.removeFakeVertex();
                leafHandler.removeFakeVertex();
                compSplitter.removeFakeVertex();
                curSolution.removeFakeVertex(graphView);
                continue;
            }
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
                std::cout << spaces << bestSolution.size() << std::endl;
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

        passageHandler.handle(graphView);

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
                if (!graphView.isIn(graph.edges[i]))
                    continue;
                graphView.removeVertex(graph.edges[i]);
                removed.push_back(graph.edges[i]);
            }
            for (int i = rem; i < (int)removed.size(); i++) {
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
