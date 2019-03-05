#include <graph/graph.h>

#include <optional>

namespace PaceVC {

struct Solution {
    std::vector<int> vertices;
};

std::optional<std::pair<int, int> > validate(const Graph& g, const Solution& s);

}
