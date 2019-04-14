#include <gtest/gtest.h>

#include <kernels/lp/lp.h>
#include <surplus/surplus.h>

using namespace PaceVC;
using namespace Kernels;

namespace {
    void doTest(Graph& graph, int sz) {
        FastZeroSurplusLPKernel(graph).reduce();
        ASSERT_GT(MinimalSurplusNonEmptySetFinder(graph).find(), (sz == 0 ? -1 : 0));
        ASSERT_EQ(sz, graph.size());
    }
}

TEST(TestLPKernel, testFastZSOneEdge) {
    Graph g(2);
    g.addEdge(0, 1);
    doTest(g, 0);
}

TEST(TestLPKernel, testFastZSEvenCycle) {
    Graph g(6);
    for (int i = 0; i < 6; i++) {
        g.addEdge(i, (i + 1) % 6);
    }
    doTest(g, 0);
}

TEST(TestLPKernel, testFastZSOddCycle) {
    Graph g(5);
    for (int i = 0; i < 5; i++) {
        g.addEdge(i, (i + 1) % 5);
    }
    doTest(g, 5);
}

TEST(TestLPKernel, testFastZSEvenCycleWithEdge) {
    Graph g(6);
    for (int i = 0; i < 6; i++) {
        g.addEdge(i, (i + 1) % 6);
    }
    g.addEdge(0, 2);
    doTest(g, 0);
}

TEST(TestLPKernel, testFastZSPartialReduce) {
    Graph g(9);
    for (int i = 0; i < 6; i++) {
        for (int j = i + 1; j < 6; j++) {
            g.addEdge(i, j);
        }
    }
    for (int i = 3; i < 6; i++) {
        for (int j = 6; j < 9; j++) {
            g.addEdge(i, j);
        }
    }
    doTest(g, 3);
}
