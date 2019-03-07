#include <gtest/gtest.h>

#include <branching/max_degree.h>
#include <graph/graph.h>
#include <solution/solution.h>

using namespace PaceVC;

TEST(TestMaxDegreeBranching, testBranchOnTwoComponents) {
    Graph g(6);
    for (int i = 0; i < 3; i++) {
        g.addEdge(i, (i + 1) % 3);
        g.addEdge(3 + i, 3 + ((i + 1) % 3));
    }

    MaxDegreeBranching<Kernels::Trivial>(g).branch();

    ASSERT_FALSE(validate(g, {g.bestSolution()}).has_value());
    ASSERT_EQ(4, g.bestSolution().size());
}

TEST(TestMaxDegreeBranching, testBranchOnComponentsAfterReduction) {
    Graph g(7);
    for (int i = 1; i < 7; i++) {
        g.addEdge(0, i);
    }
    g.addEdge(1, 2);
    g.addEdge(3, 4);
    g.addEdge(5, 6);

    MaxDegreeBranching<Kernels::Trivial>(g).branch();

    ASSERT_FALSE(validate(g, {g.bestSolution()}).has_value());
    ASSERT_EQ(4, g.bestSolution().size());
}
