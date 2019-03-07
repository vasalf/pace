#include <gtest/gtest.h>

#include <branching/cutpoints.h>
#include <graph/graph.h>

using namespace PaceVC;

TEST(TestCutpointsBranching, testSelectCutpoint) {
    Graph g(9);
    for (int i = 1; i < 5; i++) {
        g.addEdge(i, 1 + (i % 4));
        g.addEdge(i, i + 4);
        g.addEdge(0, i);
    }

    int v = CutpointsSelector<MaxDegreeSelector>(g).select();
    ASSERT_TRUE(1 <= v);
    ASSERT_TRUE(v < 5);
}
