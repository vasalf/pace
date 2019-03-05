#include <gtest/gtest.h>

#include <graph/graph.h>
#include <kernels/trivial.h>
#include <solution/solution.h>

using namespace PaceVC;

TEST(TestTrivial, testTree) {
    std::vector<int> expected = {0, 2};

    Graph g(6);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(0, 3);
    g.addEdge(2, 4);
    g.addEdge(2, 5);

    Kernels::Trivial(g).reduce();
    std::vector<int> solution = g.restoreSolution();
    std::sort(solution.begin(), solution.end());

    ASSERT_EQ(0, g.size());
    ASSERT_EQ(expected, solution);
}

TEST(TestTrivial, testSun) {
    Graph g(8);
    g.addEdge(0, 1);
    g.addEdge(2, 3);
    g.addEdge(4, 5);
    g.addEdge(6, 7);
    g.addEdge(0, 2);
    g.addEdge(2, 4);
    g.addEdge(4, 6);
    g.addEdge(6, 1);

    Kernels::Trivial(g).reduce();

    ASSERT_EQ(0, g.size());
    ASSERT_EQ(4, g.restoreSolution().size());
}

TEST(TestTrivial, testEvenCycle) {
    Graph g(8);

    for (int i = 0; i < 8; i++) {
        g.addEdge(i, (i + 1) % 8);
    }

    Kernels::Trivial(g).reduce();

    ASSERT_EQ(0, g.size());
    ASSERT_EQ(4, g.restoreSolution().size());
}

TEST(TestTrivial, testOddCycle) {
    Graph g(7);

    for (int i = 0; i < 7; i++) {
        g.addEdge(i, (i + 1) % 7);
    }

    Kernels::Trivial(g).reduce();

    ASSERT_EQ(0, g.size());
    ASSERT_EQ(4, g.restoreSolution().size());
}

TEST(TestTrivial, testConnectedOddCycles) {
    Graph g(10);

    for (int i = 0; i < 5; i++) {
        g.addEdge(i, (i + 1) % 5);
        g.addEdge(5 + i, 5 + ((i + 1) % 5));
    }
    g.addEdge(0, 5);

    Kernels::Trivial(g).reduce();

    ASSERT_EQ(0, g.size());
    ASSERT_EQ(6, g.restoreSolution().size());
}

TEST(TestTrivial, testConnectedEvenCycles) {
    Graph g(12);

    for (int i = 0; i < 6; i++) {
        g.addEdge(i, (i + 1) % 6);
        g.addEdge(6 + i, 6 + ((i + 1) % 6));
    }
    g.addEdge(0, 6);

    Kernels::Trivial(g).reduce();

    ASSERT_EQ(0, g.size());
    ASSERT_EQ(6, g.restoreSolution().size());
}

TEST(TestTrivial, testOnePathToSpan) {
    Graph g(6);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(0, 3);
    g.addEdge(0, 4);
    g.addEdge(1, 2);
    g.addEdge(3, 4);
    g.addEdge(1, 3);
    g.addEdge(2, 5);
    g.addEdge(5, 4);

    Kernels::Trivial(g).reduce();

    ASSERT_EQ(4, g.size());
}

TEST(TestTrivial, testTwoCyclesWithSharedVertex) {
    std::vector<int> expected = {1, 3, 5};

    Graph g(7);
    for (int i = 0; i < 4; i++) {
        g.addEdge(i, (i + 1) % 4);
        g.addEdge(3 + i, 3 + ((i + 1) % 4));
    }

    Kernels::Trivial(g).reduce();

    ASSERT_EQ(0, g.size());
    std::vector<int> solution = g.restoreSolution();
    std::sort(solution.begin(), solution.end());
    ASSERT_EQ(expected, solution);
}

TEST(TestTrivial, testTriangleSpan) {
    Graph g(7);
    for (int i = 0; i < 3; i++) {
        g.addEdge(i, (i + 1) % 3);
    }
    g.addEdge(1, 3);
    g.addEdge(2, 4);
    for (int i = 3; i < 7; i++) {
        for (int j = i + 1; j < 7; j++)
            g.addEdge(i, j);
    }

    Kernels::Trivial(g).reduce();

    ASSERT_EQ(4, g.size());
    g.takeVertex(3);
    g.takeVertex(4);
    g.takeVertex(5);
    g.removeVertex(6);

    ASSERT_EQ(0, g.size());
    ASSERT_EQ(5, g.restoreSolution().size());
}

TEST(TestTrivial, testOverlappingCycles) {
    Graph g(13);
    for (int i = 0; i < 13; i++)
        g.addEdge(i, (i + 1) % 13);
    g.addEdge(0, 2);
    g.addEdge(7, 9);
    g.addEdge(8, 10);
    Graph h = g;

    Kernels::Trivial(g).reduce();

    ASSERT_EQ(0, g.size());
    ASSERT_FALSE(validate(h, {g.restoreSolution()}).has_value());
}
