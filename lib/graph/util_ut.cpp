#include <gtest/gtest.h>

#include <graph/util.h>

using namespace PaceVC;

namespace {
    std::vector<int> sorted(std::vector<int> v) {
        std::sort(v.begin(), v.end());
        return v;
    }
}

TEST(TestUtil, cutpointsInStar) {
    std::vector<int> expected = {0};

    Graph g(5);
    for (int i = 1; i < 5; i++)
        g.addEdge(0, i);
    ASSERT_EQ(expected, sorted(sorted(cutpoints(g))));
}

TEST(TestUtil, cutpointsInTriangle) {
    std::vector<int> expected = {};

    Graph g(3);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);

    ASSERT_EQ(expected, sorted(cutpoints(g)));
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

    ASSERT_EQ(expected, sorted(cutpoints(g)));
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

    ASSERT_EQ(expected, sorted(cutpoints(g)));
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

    ASSERT_EQ(expected, sorted(cutpoints(g)));
}

TEST(TestUtil, cutpointsUnconnectedGraph) {
    std::vector<int> expected = {0, 4};

    Graph g(7);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(0, 3);
    g.addEdge(4, 5);
    g.addEdge(4, 6);

    ASSERT_EQ(expected, sorted(cutpoints(g)));
}

TEST(TestUtil, cutpointsFourTriangles) {
    std::vector<int> expected = {0, 1, 2};

    Graph g(9);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(0, 2);
    g.addEdge(0, 3);
    g.addEdge(0, 4);
    g.addEdge(3, 4);
    g.addEdge(1, 5);
    g.addEdge(1, 6);
    g.addEdge(5, 6);
    g.addEdge(2, 7);
    g.addEdge(2, 8);
    g.addEdge(7, 8);

    ASSERT_EQ(expected, sorted(cutpoints(g)));
}

TEST(TestUtil, cutpointsThreeTriangles) {
    std::vector<int> expected = {2, 4};

    Graph g(7);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(2, 4);
    g.addEdge(3, 4);
    g.addEdge(4, 5);
    g.addEdge(4, 6);
    g.addEdge(5, 6);

    ASSERT_EQ(expected, sorted(cutpoints(g)));
}

TEST(TestUtil, cutpointsThreeSeparatedTriangles) {
    std::vector<int> expected = {2, 3, 4, 6, 7, 8};

    Graph g(11);
    for (int i = 0; i < 3; i++) {
        g.addEdge(i, (i + 1) % 3);
        g.addEdge(4 + i, 4 + ((i + 1) % 3));
        g.addEdge(8 + i, 8 + ((i + 1) % 3));
    }
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(6, 7);
    g.addEdge(7, 8);

    ASSERT_EQ(expected, sorted(cutpoints(g)));
}

TEST(TestUtil, cutpointsFourSquares) {
    std::vector<int> expected = {0, 1, 2};

    Graph g(16);
    for (int i = 0; i < 4; i++) {
        g.addEdge(i, (i + 1) % 4);
    }
    g.addEdge(0, 2);
    g.addEdge(0, 4);
    g.addEdge(4, 5);
    g.addEdge(5, 6);
    g.addEdge(6, 0);
    g.addEdge(0, 5);
    g.addEdge(1, 7);
    g.addEdge(7, 8);
    g.addEdge(8, 9);
    g.addEdge(9, 1);
    g.addEdge(1, 8);
    g.addEdge(2, 10);
    g.addEdge(10, 11);
    g.addEdge(11, 12);
    g.addEdge(12, 2);
    g.addEdge(2, 11);

    ASSERT_EQ(expected, sorted(cutpoints(g)));
}

namespace {
    std::vector<std::vector<int>> normalized(std::vector<std::vector<int>> v) {
        for (auto& vec : v) {
            std::sort(vec.begin(), vec.end());
        }
        std::sort(v.begin(), v.end());
        return v;
    }
}

TEST(TestUtil, componentsOneComponent) {
    std::vector<std::vector<int>> expected = {{0, 1, 2, 3}};

    Graph g(4);
    g.addEdge(0, 1);
    g.addEdge(2, 3);
    g.addEdge(1, 3);

    ASSERT_EQ(expected, normalized(connectedComponents(g)));
}

TEST(TestUtil, componentsTwoComponents) {
    std::vector<std::vector<int>> expected = {{0, 1, 3}, {2, 4}};

    Graph g(5);
    g.addEdge(0, 1);
    g.addEdge(2, 4);
    g.addEdge(3, 1);
    g.addEdge(3, 0);

    ASSERT_EQ(expected, normalized(connectedComponents(g)));
}

TEST(TestUtil, componentsIsolatedVertices) {
    std::vector<std::vector<int>> expected = {{0, 1, 3}, {2}, {4, 5}, {6}};

    Graph g(7);
    g.addEdge(0, 1);
    g.addEdge(5, 4);
    g.addEdge(3, 1);
    g.addEdge(3, 0);

    ASSERT_EQ(expected, normalized(connectedComponents(g)));
}
