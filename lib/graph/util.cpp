#include <graph/util.h>

#include <algorithm>

namespace PaceVC {
namespace {
    struct CutpointFinder {
        const Graph& g;

        std::vector<int> height;
        std::vector<int> up;
        std::vector<int> cutpoints;

        int dfs(int v, int h) {
            height[v] = h;
            up[v] = h;
            int ret = 0;
            bool added = false;
            for (int u : g.adjacent(v)) {
                if (height[u] == -1) {
                    dfs(u, h + 1);
                    up[v] = std::min(up[v], up[u]);
                    ++ret;
                    if (h > 0 && up[u] >= h && !added) {
                        cutpoints.push_back(v);
                        added = true;
                    }
                } else {
                    up[v] = std::min(up[v], height[u]);
                }
            }
            return ret;
        }

        CutpointFinder(const Graph& graph)
            : g(graph)
        {}

        std::vector<int> find() {
            height.resize(g.realSize(), -1);
            up.resize(g.realSize(), 0);

            for (int start : g.undecided()) {
                if (height[start] > -1) {
                    continue;
                }
                if (dfs(start, 0) > 1) {
                    cutpoints.push_back(start);
                }
            }

            return cutpoints;
        }
    };
}

std::vector<int> cutpoints(const Graph& graph) {
    return CutpointFinder(graph).find();
}

namespace {
    struct ConnectedComponentsFinder {
        const Graph& g;
        std::vector<bool> vis;
        std::vector<std::vector<int>> components;

        void dfs(int u) {
            vis[u] = true;
            components.back().push_back(u);
            for (int v : g.adjacent(u)) {
                if (!vis[v]) {
                    dfs(v);
                }
            }
        }

        ConnectedComponentsFinder(const Graph& graph)
            : g(graph)
        {}

        std::vector<std::vector<int>> find() {
            vis.resize(g.realSize(), false);
            for (int u : g.undecided()) {
                if (!vis[u]) {
                    components.push_back(std::vector<int>());
                    dfs(u);
                }
            }
            return components;
        }
    };
}

std::vector<std::vector<int>> connectedComponents(const Graph& graph) {
    return ConnectedComponentsFinder(graph).find();
}

}
