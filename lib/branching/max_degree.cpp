#include <branching/max_degree.h>

#include <algorithm>

namespace PaceVC {

MaxDegreeSelector::MaxDegreeSelector(const Graph& graph)
    : g(graph)
{}

int MaxDegreeSelector::select() {
    return *std::max_element(g.undecided().begin(), g.undecided().end(),
        [this](int i, int j) {
            return g.adjacent(i).size() < g.adjacent(j).size();
        }
    );
}

}
