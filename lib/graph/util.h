#pragma once

#include <graph/graph.h>

#include <vector>

namespace PaceVC {

std::vector<int> cutpoints(const Graph& graph);

std::vector<std::vector<int>> connectedComponents(const Graph& g);

}
