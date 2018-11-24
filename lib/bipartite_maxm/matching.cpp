#include <bipartite_maxm/matching.h>

#include <limits>
#include <queue>

namespace PaceVC {

HopcroftKarpMatchingFinder::HopcroftKarpMatchingFinder(const BipartiteGraph& g)
    : graph(g), pair(g.rightSize(), -1), pairRight(g.leftSize(), -1), dist(g.leftSize()), edgeptr(g.leftSize()) {}

bool HopcroftKarpMatchingFinder::bfs() {
    std::queue<int> q;
    dist.assign(dist.size(), std::numeric_limits<int>::max());
    for (int i = 0; i < graph.leftSize(); i++) {
        if (pairRight[i] == -1) {
            dist[i] = 0;
            q.push(i);
        }
    }
    bool ret = false;
    while (!q.empty()) {
        int a = q.front();
        q.pop();
        for (int v : graph.neighboursOfLeft(a)) {
            if (pair[v] == -1) {
                ret = true;
                continue;
            }
            if (dist[pair[v]] == std::numeric_limits<int>::max()) {
                dist[pair[v]] = dist[a] + 1;
                q.push(pair[v]);
            }
        }
    }
    return ret;
}

bool HopcroftKarpMatchingFinder::dfs(int u) {
    for (; edgeptr[u] < (int)graph.neighboursOfLeft(u).size(); edgeptr[u]++) {
        int v = graph.neighboursOfLeft(u)[edgeptr[u]];
        if (pair[v] == -1 || (dist[pair[v]] == dist[u] + 1 && dfs(pair[v]))) {
            pair[v] = u;
            pairRight[u] = v;
            edgeptr[u]++;
            return true;
        }
    }
    return false;
}

int HopcroftKarpMatchingFinder::find() {
    int ans = 0;
    while (bfs()) {
        edgeptr.assign(graph.leftSize(), 0);
        for (int i = 0; i < graph.leftSize(); i++) {
            if (pairRight[i] == -1) {
                if (dfs(i))
                    ans++;
            }
        }
    }
    return ans;
}

}
