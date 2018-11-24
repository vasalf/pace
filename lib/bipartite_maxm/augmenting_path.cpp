#include <bipartite_maxm/augmenting_path.h>

namespace PaceVC {

bool KuhnAugmentingPathFinder::find(int v) {
    if (visLeft[v])
        return false;
    visLeft[v] = true;
    for (int u : graph.neighboursOfLeft(v)) {
        visRight[u] = true;
        if (pair[u] == -1 || find(pair[u]))
            return true;
    }
    return false;
}

}
