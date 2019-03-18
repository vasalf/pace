#pragma once

#include <graph/graph.h>

namespace PaceVC {

class ChenLiuJia3DVC {
    using TThis = ChenLiuJia3DVC;

    Graph& g;

    void fold(int v);
    void branchOn(int v);
    void doBranch();

public:
    ChenLiuJia3DVC(Graph& graph);

    void branch();
};

}
