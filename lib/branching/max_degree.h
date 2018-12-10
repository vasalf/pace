#pragma once

#include <graph/graph.h>

#include <algorithm>

namespace PaceVC {

template<class IntermediateReducer>
class MaxDegreeBranching {
public:

    MaxDegreeBranching(Graph& g)
        : graph(g)
    {}

    void branch() {
        doBranch(graph);
    }

private:
    Graph& graph;

    void doBranch(Graph curCopy) {
        IntermediateReducer(curCopy).reduce();

        if (curCopy.size() == 0) {
            curCopy.restoreSolution();
            graph.saveSolution(curCopy.solution());
            return;
        }

        int v = *std::max_element(curCopy.undecided().begin(), curCopy.undecided().end(),
            [&curCopy](int u, int v) {
                return curCopy.adjacent(u).size() < curCopy.adjacent(v).size();
            }
        );

        {
            Graph next(curCopy);
            next.takeVertex(v);
            doBranch(std::move(next));
        }

        {
            Graph next(curCopy);
            for (int u : curCopy.adjacent(v)) {
                next.takeVertex(u);
            }
            doBranch(std::move(next));
        }
    }
};

}
