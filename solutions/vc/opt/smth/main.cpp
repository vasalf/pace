#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

namespace utility {
    template<class It, class Eq>
    It unique_swap(It b, It e, const Eq& equal) {
        if (b == e) {
            return e;
        }
        It ret = b++;
        while (b != e) {
            if (!equal(*ret, *b)) {
                ret++;
                std::swap(*ret, *b);
            }
            b++;
        }
        return ++ret;
    }
}

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
    std::vector<int> initialFirstEdge;
    std::vector<int> firstEdge;
    std::vector<int> lastEdge;
    std::vector<int> edges;
    std::vector<int> id;
    std::vector<EdgePosInfo> edgeInfos;
    std::vector<int> ubuf, rbuf;
    std::vector<int> ebuf, idbuf;

    Graph(int n, int m)
            : n(n)
              , initialFirstEdge(n)
              , firstEdge(n)
              , lastEdge(n)
              , edges(2 * m)
              , id(2 * m)
    {
        initialFirstEdge.reserve(2 * n);
        firstEdge.reserve(2 * n);
        lastEdge.reserve(2 * n);
        edgeInfos.reserve(m);
        ubuf.reserve(m);
        rbuf.reserve(m);
        ebuf.reserve(m);
        idbuf.reserve(m);
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
        std::memcpy(initialFirstEdge.data(), firstEdge.data(), sizeof(int) * n);
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
        initialFirstEdge.push_back(edges.size());
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
        edges.resize(initialFirstEdge[last]);
        id.resize(initialFirstEdge[last]);
        initialFirstEdge.pop_back();
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

    void uniqueizeEdgesOf(int v) {
        ubuf.resize(getDegree(v));
        std::iota(ubuf.begin(), ubuf.end(), firstEdge[v]);
        std::sort(ubuf.rbegin(), ubuf.rend(),
            [this](int i, int j) {
                return edges[i] < edges[j];
            }
        );
        int mid = ubuf.rend() - utility::unique_swap(ubuf.rbegin(), ubuf.rend(),
            [this](int i, int j) {
                return edges[i] == edges[j];
            }
        );
        rbuf.resize(getDegree(v));
        for (int i = 0; i < getDegree(v); i++)
            rbuf[ubuf[i] - firstEdge[v]] = i;
        ebuf.resize(getDegree(v));
        idbuf.resize(getDegree(v));
        std::memcpy(ebuf.data(), edges.data() + firstEdge[v], getDegree(v) * sizeof(int));
        std::memcpy(idbuf.data(), id.data() + firstEdge[v], getDegree(v) * sizeof(int));
        for (int i = 0; i < getDegree(v); i++) {
            edges[rbuf[i] + firstEdge[v]] = ebuf[i];
            id[rbuf[i] + firstEdge[v]] = idbuf[i];
            edgeInfos[idbuf[i]].that(i + firstEdge[v]) = rbuf[i] + firstEdge[v];
        }
        firstEdge[v] += mid;
        for (int i = initialFirstEdge[v]; i < firstEdge[v]; i++) {
            removeEdge(edges[i], edgeInfos[id[i]].other(i));
        }
    }

    void revertFakeVertexUniqueization() {
        for (int i = initialFirstEdge[n - 1]; i < firstEdge[n - 1]; i++) {
            edges[lastEdge[edges[i]]] = n - 1;
            id[lastEdge[edges[i]]] = id[i];
            edgeInfos[id[i]].other(i) = lastEdge[edges[i]]++;
        }
        firstEdge[n - 1] = initialFirstEdge[n - 1];
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
        SolutionStorage ret(status.size(), {});
        for (auto& s : ret.status)
            s = VertexStatus::IGNORED;
        for (int u : graphView.leftVertices)
            ret.status[u] = VertexStatus::FREE;
        return ret;
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

struct FastLPKernel {
    Graph& graph;

    std::vector<int> pairOfLeft;
    std::vector<int> pairOfRight;

    FastLPKernel(Graph& g)
        : graph(g)
        , pairOfLeft(g.n, -1)
        , pairOfRight(g.n, -1)
        , dist(g.n)
        , edgeptr(g.n)
        , wasLast(g.n, 0)
        , rightEndInVC(g.n)
        , vis(g.n)
        , compId(g.n, -1)
    {
        pairOfLeft.reserve(2 * g.n);
        pairOfRight.reserve(2 * g.n);
        dist.reserve(2 * g.n);
        qbuf.reserve(2 * g.n);
        edgeptr.reserve(2 * g.n);
        rightEndInVC.reserve(2 * g.n);
        vis.reserve(2 * g.n);
        compId.reserve(2 * g.n);
        topsort.reserve(2 * g.n);
        wasLast.reserve(2 * g.n);
    }

    std::vector<int> dist;
    std::vector<int> qbuf;
    std::vector<int> edgeptr;
    std::vector<int> wasLast;
    int iteration = 0;

    inline bool hasPairOfLeft(int v) {
        return !(pairOfLeft[v] == -1 || pairOfLeft[v] >= graph.n || wasLast[pairOfLeft[v]] != iteration);
    }

    inline bool hasPairOfRight(int v) {
        return !(pairOfRight[v] == -1 || pairOfRight[v] >= graph.n || wasLast[pairOfRight[v]] != iteration);
    }

    bool hopcroftKarpBfs(GraphViewRef& graphView) {
        qbuf.clear();
        int qhead = 0;
        for (int u : graphView.leftVertices) {
            dist[u] = -1;
            if (!hasPairOfLeft(u)) {
                dist[u] = 0;
                qbuf.push_back(u);
            }
        }
        bool ret = false;
        while (qhead < (int)qbuf.size()) {
            int a = qbuf[qhead++];
            for (int i = graph.firstEdge[a]; i < graph.lastEdge[a]; i++) {
                int v = graph.edges[i];
                if (!hasPairOfRight(v)) {
                    ret = true;
                    continue;
                }
                if (dist[pairOfRight[v]] == -1) {
                    dist[pairOfRight[v]] = dist[a] + 1;
                    qbuf.push_back(pairOfRight[v]);
                }
            }
        }
        return ret;
    }

    bool hopcroftKarpDfs(int u) {
        for (; edgeptr[u] < graph.lastEdge[u]; edgeptr[u]++) {
            int v = graph.edges[edgeptr[u]];
            if (!hasPairOfRight(v) || (dist[pairOfRight[v]] == dist[u] + 1 && hopcroftKarpDfs(pairOfRight[v]))) {
                pairOfRight[v] = u;
                pairOfLeft[u] = v;
                edgeptr[u]++;
                return true;
            }
        }
        return false;
    }

    std::vector<bool> rightEndInVC;
    std::vector<bool> vis;

    void konig(GraphViewRef& graphView) {
        qbuf.clear();
        int qhead = 0;
        for (int u : graphView.leftVertices) {
            vis[u] = false;
            if (!hasPairOfLeft(u)) {
                qbuf.push_back(u);
                vis[u] = true;
            }
        }
        while (qhead < (int)qbuf.size()) {
            int a = qbuf[qhead++];
            for (int i = graph.firstEdge[a]; i < graph.lastEdge[a]; i++) {
                int v = graph.edges[i];
                rightEndInVC[pairOfRight[v]] = true;
                if (!vis[pairOfRight[v]]) {
                    vis[pairOfRight[v]] = true;
                    qbuf.push_back(pairOfRight[v]);
                }
            }
        }
    }

    inline bool isUndecided(int u) {
        return hasPairOfLeft(u) && !rightEndInVC[u] && hasPairOfRight(u) && !rightEndInVC[pairOfRight[u]];
    }

    std::vector<int> topsort;
    void topsortDfs(int v) {
        vis[v] = true;
        for (int i = graph.firstEdge[v]; i < graph.lastEdge[v]; i++) {
            if (graph.edges[i] == pairOfLeft[v])
                continue;
            if (!hasPairOfRight(graph.edges[i]))
                continue;
            int u = pairOfRight[graph.edges[i]];
            if (!isUndecided(u))
                continue;
            if (!vis[u])
                topsortDfs(u);
        }
        topsort.push_back(v);
    }

    std::vector<int> compId;
    void markDfs(int v, int mark) {
        compId[v] = mark;
        int vh = pairOfLeft[v];
        for (int i = graph.firstEdge[vh]; i < graph.lastEdge[vh]; i++) {
            if (graph.edges[i] == v)
                continue;
            if (!isUndecided(graph.edges[i]))
                continue;
            if (compId[graph.edges[i]] == -1) {
                markDfs(graph.edges[i], mark);
            }
        }
    }

    int update(GraphViewRef& graphView) {
        iteration++;
        for (int u : graphView.leftVertices) {
            wasLast[u] = iteration;
        }
        for (int u : graphView.leftVertices) {
            if (hasPairOfLeft(u)) {
                pairOfRight[pairOfLeft[u]] = u;
            }
        }
        for (int u : graphView.leftVertices) {
            if (hasPairOfRight(u)) {
                if (pairOfLeft[pairOfRight[u]] != u)
                    pairOfRight[u] = -1;
            }
        }
        for (int u : graphView.leftVertices) {
            if (hasPairOfLeft(u)) {
                if (pairOfRight[pairOfLeft[u]] != u)
                    pairOfLeft[u] = -1;
            }
        }
        int ans = 0;
        while (ans < (int)graphView.leftVertices.size() && hopcroftKarpBfs(graphView)) {
            for (int u : graphView.leftVertices) {
                edgeptr[u] = graph.firstEdge[u];
            }
            for (int u : graphView.leftVertices) {
                if (!hasPairOfLeft(u)) {
                    if (hopcroftKarpDfs(u))
                        ans++;
                }
            }
        }
        for (int u : graphView.leftVertices) {
            rightEndInVC[u] = false;
        }
        konig(graphView);
        for (int u : graphView.leftVertices) {
            vis[u] = false;
            compId[u] = -1;
        }
        topsort.clear();
        for (int u : graphView.leftVertices) {
            if (isUndecided(u)) {
                if (!vis[u])
                    topsortDfs(u);
            }
        }
        std::reverse(topsort.begin(), topsort.end());
        int comp = 0;
        for (int u : topsort) {
            if (compId[u] == -1 && vis[u])
                markDfs(u, comp++);
        }
        return ans;
    }

    void addFakeVertex() {
        pairOfLeft.push_back(-1);
        pairOfRight.push_back(-1);
        dist.push_back(-1);
        edgeptr.push_back(0);
        rightEndInVC.push_back(false);
        vis.push_back(false);
        compId.push_back(false);
        wasLast.push_back(0);
    }

    void removeFakeVertex() {
        if (pairOfLeft.back() != -1 && pairOfLeft.back() < (int)pairOfRight.size()) {
            pairOfRight[pairOfLeft.back()] = -1;
        }
        if (pairOfRight.back() != -1 && pairOfRight.back() < (int) pairOfLeft.size()) {
            pairOfLeft[pairOfRight.back()] = -1;
        }
        pairOfLeft.pop_back();
        pairOfRight.pop_back();
        dist.pop_back();
        edgeptr.pop_back();
        rightEndInVC.pop_back();
        vis.pop_back();
        compId.pop_back();
        wasLast.pop_back();
    }

    enum class VertexResolution {
        TAKE,
        REMOVE,
        LEAVE
    };

    inline VertexResolution vertexResolution(int v) {
        if (!hasPairOfLeft(v) || !hasPairOfRight(v)) {
            return VertexResolution::REMOVE;
        } else if (!rightEndInVC[v] && rightEndInVC[pairOfRight[v]]) {
            return VertexResolution::TAKE;
        } else if (rightEndInVC[v] && !rightEndInVC[pairOfRight[v]]) {
            return VertexResolution::REMOVE;
        } else if (compId[v] < compId[pairOfLeft[v]]) {
            return VertexResolution::TAKE;
        } else if (compId[v] > compId[pairOfLeft[v]]) {
            return VertexResolution::REMOVE;
        }
        return VertexResolution::LEAVE;
    }
};

struct LeafHandler {
    Graph& graph;
    FastLPKernel& lpKernel;

    int iteration = 0;

    std::vector<int> qbuf;
    std::vector<int> removedAtIteration;
    std::vector<int>& removed;
    SolutionStorage& curSolution;

    LeafHandler(Graph& g, FastLPKernel& lpKernel, std::vector<int>& removed, SolutionStorage& curSolution)
        : graph(g)
        , lpKernel(lpKernel)
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
    FastLPKernel& fastLPKernel;
    std::vector<int>& removed;

    std::vector<int> leftVBuf;

    PassageHandler(Graph& graph,
                   ComponentSplitter& cs,
                   LeafHandler& lh,
                   SolutionStorage& ss,
                   FastLPKernel& flk,
                   std::vector<int>& removed)
        : graph(graph)
        , componentSplitter(cs)
        , leafHandler(lh)
        , solutionStorage(ss)
        , fastLPKernel(flk)
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
                graph.uniqueizeEdgesOf(graph.n - 1);
                removed.push_back(-2);
                removed.push_back(u);
                removed.push_back(v);
                removed.push_back(-2);
                componentSplitter.addFakeVertex();
                leafHandler.addFakeVertex();
                fastLPKernel.addFakeVertex();
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

    FastLPKernel fastLPKernel;

    PassageHandler passageHandler;

    int curDepth = -1;

#ifdef DEBUG_BRANCHING
    std::string spaces = "c  ";
#endif

public:
    Brancher(Graph& g, std::vector<int> aprior)
        : graph(g)
        , compSplitter(g)
        , leafHandler(g, fastLPKernel, removed, curSolution)
        , bestSolution(graph.n)
        , curSolution(g.n, aprior)
        , fastLPKernel(g)
        , passageHandler(graph, compSplitter, leafHandler, curSolution, fastLPKernel, removed)
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
                graph.revertFakeVertexUniqueization();
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
                fastLPKernel.removeFakeVertex();
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

        int lpReduced = 0;
        fastLPKernel.update(graphView);
        for (int i = 0; i < (int)graphView.leftVertices.size(); ) {
            int u = graphView.leftVertices[i];
            auto resolution = fastLPKernel.vertexResolution(u);
            if (resolution == FastLPKernel::VertexResolution::TAKE) {
                curSolution.push(u);
                removed.push_back(u);
                graph.removeVertex(u);
                graphView.removeVertex(u);
                ++lpReduced;
            } else if (resolution == FastLPKernel::VertexResolution::REMOVE) {
                removed.push_back(u);
                graph.removeVertex(u);
                graphView.removeVertex(u);
                ++lpReduced;
            } else {
                i++;
            }
        }

        if (curSolution.size() + (graphView.leftVertices.size() + 1) / 2 > bestSolution.size()) {
#ifdef DEBUG_BRANCHING
            std::cout << spaces << "lp_cutoff(cur=" << curSolution.size()
                      << ",curSize=" << graphView.leftVertices.size()
                      << ",best=" << bestSolution.size()
                      << ",lpReduced=" << lpReduced
                      << ")" << std::endl;
#endif
            return;
        }

        for (int i = 0; i < (int)graphView.leftVertices.size(); ) {
            int v = graphView.leftVertices[i];
            if (graph.getDegree(v) > (int)bestSolution.size() - (int)curSolution.size()) {
                curSolution.push(v);
                removed.push_back(v);
                graph.removeVertex(v);
                graphView.removeVertex(v);
            } else {
                i++;
            }
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
                std::cout << spaces << "component, currently took " << curSolution.size() << std::endl;
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
#ifdef DEBUG_BRANCHING
                std::cout << spaces << "components: better solution(" << curSolution.size() << ")" << std::endl;
#endif
                curSolution.saveTo(bestSolution);
                assert(bestSolution.size() == curSolution.size());
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
        std::cout << spaces << "inner(size=" << graphView.leftVertices.size() << ",lpReduced=" << lpReduced << ")" << std::endl;
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
    for (int i = 0; i < n; i++) {
        g.uniqueizeEdgesOf(i);
    }

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
