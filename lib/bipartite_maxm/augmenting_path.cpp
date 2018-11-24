#include <bipartite_maxm/augmenting_path.h>

namespace PaceVC {

bool KuhnAugmentingPathFinder::find(int v) {
    if (visLeft[v])
        return false;
    visLeft[v] = true;
    for (int u : graph.neighboursOfLeft(v)) {
        visRight[u] = true;
        if (pair[u] == -1 || find(pair[u])) {
            pair[u] = v;
            pairRight[v] = u;
            return true;
        }
    }
    return false;
}

bool OptimizedKuhnAugmentingPathFinder::find(int v) {
    if (visLeft[v])
        return false;
    visLeft[v] = true;
    for (int u : graph.neighboursOfLeft(v)) {
        visRight[u] = true;
        if (pair[u] == -1) {
            pair[u] = v;
            pairRight[v] = u;
            return true;
        }
    }
    for (int u : graph.neighboursOfLeft(v)) {
        visRight[u] = true;
        if (find(pair[u])) {
            pair[u] = v;
            pairRight[v] = u;
            return true;
        }
    }
    return false;
}

}
