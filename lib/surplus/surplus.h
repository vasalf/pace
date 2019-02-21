#pragma once

#include <graph/graph.h>

namespace PaceVC {

struct MinimalSurplusSetFinder {
    const Graph& g;
    std::vector<int> set;
    std::vector<int> neighbours;

    MinimalSurplusSetFinder(const Graph& graph);

    int find();
};

struct MinimalSurplusNonEmptySetFinder {
    const Graph& g;
    std::vector<int> set;
    std::vector<int> neighbours;

    MinimalSurplusNonEmptySetFinder(const Graph& graph);

    int find();
};

/// Returns 2 * {LPVC answer}
int findLPVCValue(const Graph& graph);

}
