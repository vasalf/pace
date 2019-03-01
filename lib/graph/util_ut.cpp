#include <gtest/gtest.h>

#include <graph/util.h>

using namespace PaceVC;

TEST(TestUtil, cutpointsInStar) {
    std::vector<int> expected = {0};

    Graph g(5);
    for (int i = 1; i < 5; i++)
        g.addEdge(0, i);
    ASSERT_EQ(expected, cutpoints(g));
}

TEST(TestUtil, cutpointsInTriangle) {
    std::vector<int> expected = {};

    Graph g(3);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);

    ASSERT_EQ(expected, cutpoints(g));
}

TEST(TestUtil, cutpointsInTwoCycles) {
    std::vector<int> expected = {4};

    Graph g(9);
    for (int i = 0; i < 5; i++) {
        g.addEdge(i, (i + 1) % 5);
    }
    for (int i = 4; i < 9; i++) {
        g.addEdge(i, 4 + (i - 4 + 1) % 5);
    }

    ASSERT_EQ(expected, cutpoints(g));
}

TEST(TestUtil, cutpointsInThreeCycles) {
    std::vector<int> expected = {0, 1};

    Graph g(7);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);
    g.addEdge(0, 3);
    g.addEdge(0, 4);
    g.addEdge(3, 4);
    g.addEdge(1, 5);
    g.addEdge(1, 6);
    g.addEdge(5, 6);

    ASSERT_EQ(expected, cutpoints(g));
}

TEST(TestUtil, cutpointsThreeComponents) {
    std::vector<int> expected = {0};

    Graph g(7);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);
    g.addEdge(0, 3);
    g.addEdge(0, 4);
    g.addEdge(3, 4);
    g.addEdge(0, 5);
    g.addEdge(0, 6);
    g.addEdge(5, 6);

    ASSERT_EQ(expected, cutpoints(g));
}

TEST(TestUtil, cutpointsUnconnectedGraph) {
    std::vector<int> expected = {0, 4};

    Graph g(7);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(0, 3);
    g.addEdge(4, 5);
    g.addEdge(4, 6);

    ASSERT_EQ(expected, cutpoints(g));
}
