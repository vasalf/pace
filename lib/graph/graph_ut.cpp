#include <gtest/gtest.h>

#include <graph/graph.h>

#include <algorithm>
#include <sstream>

using namespace PaceVC;

TEST(TestGraph, emptyGraph) {
    Graph g(0);

    ASSERT_EQ(0, g.size());
    ASSERT_EQ(0, g.realSize());
    ASSERT_TRUE(g.solution().empty());
    ASSERT_TRUE(g.undecided().empty());
    ASSERT_TRUE(g.removed().empty());

    g.squeeze();

    ASSERT_EQ(0, g.size());
    ASSERT_EQ(0, g.realSize());
    ASSERT_TRUE(g.solution().empty());
    ASSERT_TRUE(g.undecided().empty());
    ASSERT_TRUE(g.removed().empty());
}

TEST(TestGraph, reduction) {
    std::vector<int> secretSolution = {
        0, 1, 6, 7, 8, 9, 10
    };

    Graph g1(12);
    for (int i = 6; i < 12; i++)
        for (int j = i + 1; j < 12; j++)
            g1.addEdge(i, j);
    for (int i = 2; i < 6; i++)
        for (int j = 6; j < 10; j++)
            g1.addEdge(i, j);
    for (int i = 0; i < 2; i++)
        for (int j = 2; j < 6; j++)
            g1.addEdge(i, j);

    Graph g2(8);
    for (int i = 2; i < 8; i++)
        for (int j = i + 1; j < 8; j++)
            g2.addEdge(i, j);
    for (int i = 0; i < 2; i++)
        for (int j = 2; j < 6; j++)
            g2.addEdge(i, j);

    g1.addReduction([](Graph& oldGraph, const Graph& nextStepRes) {
        if (std::count(nextStepRes.solution().begin(), nextStepRes.solution().end(), 0)) {
            for (int i = 0; i < 2; i++)
                oldGraph.removeVertex(i);
            for (int i = 2; i < 6; i++)
                oldGraph.takeVertex(i);
        } else {
            for (int i = 0; i < 2; i++)
                oldGraph.takeVertex(i);
            for (int i = 2; i < 6; i++)
                oldGraph.removeVertex(i);
        }

        for (int u : nextStepRes.solution()) {
            if (u >= 2)
                oldGraph.takeVertex(u + 4);
        }
        for (int u : nextStepRes.removed()) {
            if (u >= 2)
                oldGraph.removeVertex(u + 4);
        }
    }, g2);

    g1.removeVertex(0);
    g1.removeVertex(1);
    for (int i = 2; i < 6; i++)
        g1.takeVertex(i);
    g1.takeVertex(6);
    g1.removeVertex(7);

    ASSERT_EQ(secretSolution, g1.restoreSolution());
}

TEST(TestGraph, simpleReduction) {
    std::vector<int> secretSolution = {6};

    Graph g(8);
    for (int i = 2; i < 8; i++)
        for (int j = i + 1; j < 8; j++)
            g.addEdge(i, j);
    for (int i = 0; i < 2; i++)
        for (int j = 2; j < 6; j++)
            g.addEdge(i, j);

    g.addSimpleReduction({0, 1, 6, 7});

    ASSERT_TRUE(g.adjacent(0).empty());
    ASSERT_TRUE(g.adjacent(1).empty());
    ASSERT_EQ(1, g.adjacent(2).size());
    ASSERT_EQ(1, g.adjacent(3).size());
    ASSERT_TRUE(g.adjacent(2).count(3));
    ASSERT_TRUE(g.adjacent(3).count(2));

    g.removeVertex(0);
    g.removeVertex(1);
    g.takeVertex(2);
    g.removeVertex(3);

    ASSERT_EQ(secretSolution, g.restoreSolution());
}

TEST(TestGraph, squeeze) {
    Graph g(8);
    for (int i = 2; i < 8; i++)
        for (int j = i + 1; j < 8; j++)
            g.addEdge(i, j);
    for (int i = 0; i < 2; i++)
        for (int j = 2; j < 6; j++)
            g.addEdge(i, j);

    for (int i = 2; i < 6; i++)
        g.takeVertex(i);

    g.squeeze();

    ASSERT_EQ(4, g.size());

    int doubledEdges = 0;
    for (int i = 0; i < 4; i++)
        doubledEdges += g.adjacent(i).size();

    ASSERT_EQ(2, doubledEdges);
}

TEST(TestGraph, realSize) {
    Graph g(8);
    for (int i = 2; i < 8; i++)
        for (int j = i + 1; j < 8; j++)
            g.addEdge(i, j);
    for (int i = 0; i < 2; i++)
        for (int j = 2; j < 6; j++)
            g.addEdge(i, j);

    for (int i = 2; i < 6; i++)
        g.takeVertex(i);

    ASSERT_EQ(g.size(), 4);
    ASSERT_EQ(g.realSize(), 8);
}

TEST(TestGraph, copyConstruct) {
    Graph g(2);
    g.addEdge(0, 1);

    Graph h(g);

    g.takeVertex(0);

    ASSERT_EQ(2, h.undecided().size());
}

TEST(TestGraph, copyAssignment) {
    Graph g(2);
    g.addEdge(0, 1);

    Graph h = g;

    g.takeVertex(0);

    ASSERT_EQ(2, h.undecided().size());
}

TEST(TestGraph, moveConstruct) {
    Graph g(2);
    g.addEdge(0, 1);

    Graph h(std::move(g));

    ASSERT_EQ(2, h.size());
    ASSERT_EQ(1, h.adjacent(0).size());
}

TEST(TestGraph, moveAssignment) {
    Graph h(0);

    {
        Graph g(2);
        g.addEdge(0, 1);
        h = std::move(g);
    }

    ASSERT_EQ(2, h.size());
    ASSERT_EQ(1, h.adjacent(0).size());
}

TEST(TestGraph, readGraph) {
    std::vector<std::unordered_set<int> > secretNeighbours = {
        {1, 2},
        {0, 3},
        {0, 3},
        {1, 2}
    };

    std::stringstream ss;
    ss << "p td 4 4" << std::endl
       << "1 2" << std::endl
       << "2 4" << std::endl
       << "4 3" << std::endl
       << "3 1" << std::endl;

    Graph g = readGraph(ss);

    std::vector<std::unordered_set<int> > adj(4);
    for (int i = 0; i < 4; i++)
        adj[i] = g.adjacent(i);

    ASSERT_EQ(secretNeighbours, adj);
}

TEST(TestGraph, testSelfLoops) {
    Graph g(3);

    g.addEdge(0, 0);
    g.addEdge(1, 1);
    g.addEdge(0, 1);
    g.addEdge(0, 0);

    g.removeVertex(2);

    ASSERT_EQ(2, g.solution().size());
    ASSERT_EQ(1, g.removed().size());
}

TEST(TestGraph, testGraphMarks) {
    Graph g(6);

    g.addEdge(0, 3);
    g.addEdge(1, 4);
    g.addEdge(2, 5);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 0);

    g.takeVertex(0);
    g.removeVertex(3);

    ASSERT_EQ(4, g.size());

    g.placeMark();

    g.takeVertex(1);
    g.removeVertex(4);
    
    ASSERT_EQ(2, g.size());

    g.placeMark();

    g.takeVertex(5);
    g.removeVertex(2);

    ASSERT_EQ(0, g.size());

    {
        Graph h = g;
        h.restoreSolution();
        g.saveSolution(h.solution());
    }

    g.restoreMark();
    ASSERT_EQ(2, g.size());

    g.restoreMark();
    ASSERT_EQ(4, g.size());

    ASSERT_EQ(3, g.bestSolution().size());
}
