#pragma once

#include <branching/branching.h>

namespace PaceVC {

struct MaxDegreeSelector {
    const Graph& g;

    MaxDegreeSelector(const Graph& graph);

    int select();
};

template<class IntermediateReducer>
using MaxDegreeBranching = Branching<IntermediateReducer, MaxDegreeSelector>;

}
