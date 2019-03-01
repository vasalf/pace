#include <graph/util.h>

#include <algorithm>

namespace PaceVC {
namespace {

struct CutpointFinder {
    const Graph& g;

    std::vector<int> height;
    std::vector<int> up;

    int dfs(int v, int h) {
        height[v] = h;
        up[v] = h;
        int ret = 0;
        for (int u : g.adjacent(v)) {
            if (height[u] == -1) {
                dfs(u, h + 1);
                up[v] = std::min(up[v], up[u]);
                ++ret;
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

        std::vector<int> cutpoints;
        std::vector<bool> was_start(g.realSize(), false);

        for (int start : g.undecided()) {
            if (height[start] > -1) {
                continue;
            }
            was_start[start] = true;
            if (dfs(start, 0) > 1) {
                cutpoints.push_back(start);
            }
        }

        for (int u : g.undecided()) {
            if (was_start[u])
                continue;
            for (int v : g.adjacent(u)) {
                if (height[v] > height[u] && up[v] >= height[u]) {
                    cutpoints.push_back(u);
                    break;
                }
            }
        }

        std::sort(cutpoints.begin(), cutpoints.end());

        return cutpoints;
    }
};

}

std::vector<int> cutpoints(const Graph& graph) {
    return CutpointFinder(graph).find();
}

}
