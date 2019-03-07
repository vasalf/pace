#pragma once

#include <branching/branching.h>
#include <branching/max_degree.h>
#include <graph/graph.h>
#include <graph/util.h>

namespace PaceVC {

template<class NoCutpointsSelector>
struct CutpointsSelector {
    const Graph& g;

    CutpointsSelector(const Graph& graph)
        : g(graph)
    {}

    int select() {
        auto cuts = cutpoints(g);
        if (cuts.empty()) {
            return NoCutpointsSelector(g).select();
        }
        return cuts[0];
    }
};

template<class IntermediateReducer, class NoCutpointsSelector = MaxDegreeSelector>
using CutpointsBranching = Branching<IntermediateReducer, CutpointsSelector<NoCutpointsSelector>>;

}
