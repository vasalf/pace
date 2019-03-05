#include <solution/solution.h>

namespace PaceVC {

std::optional<std::pair<int, int> > validate(const Graph& g, const Solution& s) {
    std::vector<bool> inSolution(g.realSize());
    for (int u : s.vertices) {
        inSolution[u] = true;
    }

    for (int i = 0; i < g.realSize(); i++) {
        for (int v : g.adjacent(i)) {
            if (!inSolution[i] && !inSolution[v]) {
                return std::make_pair(i, v);
            }
        }
    }
    return {};
}

}
