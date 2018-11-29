#include <gtest/gtest.h>

#include <surplus/surplus.h>

#include <graph/graph.h>

#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

using namespace PaceVC;

namespace {

std::unordered_set<int> neighbours(const Graph& g, std::vector<int> set) {
    std::unordered_set<int> retSet;
    for (int u : set) {
        for (int v : g.adjacent(u)) {
            retSet.insert(v);
        }
    }

    return retSet;
}

template<class SurplusFinder>
bool checkSurplusSets(const Graph& g, const SurplusFinder& sfinder) {
    std::unordered_set<int> set;
    for (int u : sfinder.set)
        set.insert(u);

    for (int u : g.undecided())
        for (int v : g.adjacent(u))
            if (set.count(u) && set.count(v))
                return false;

    std::unordered_set<int> neighboursSet;
    for (int u : sfinder.neighbours)
        neighboursSet.insert(u);

    return neighbours(g, sfinder.set) == neighboursSet;
}

template<class SurplusFinder>
void assertSurplusImpl(int expected, const Graph& g) {
    SurplusFinder sfinder(g);
    ASSERT_EQ(expected, sfinder.find());
    ASSERT_TRUE(checkSurplusSets(g, sfinder));
}

void assertSurplus(int expected, const Graph& g) {
    assertSurplusImpl<MinimalSurplusSetFinder>(expected < 0 ? expected : 0, g);
    assertSurplusImpl<MinimalSurplusNonEmptySetFinder>(expected, g);
}

Graph inputGraph(const std::string s) {
    std::stringstream ss;
    ss << s;
    return readGraph(ss);
}

}

TEST(TestSurplus, evenCycle) {
    std::string test =
    "p td 6 6\n"
    "1 2\n"
    "2 3\n"
    "3 4\n"
    "4 5\n"
    "5 6\n"
    "6 1\n";

    assertSurplus(0, inputGraph(test));
}

TEST(TestSurplus, oddCycle) {
    std::string test =
    "p td 5 5\n"
    "1 2\n"
    "2 3\n"
    "3 4\n"
    "4 5\n"
    "5 1\n";

    assertSurplus(1, inputGraph(test));
}

TEST(TestSurplus, triangleWithLeaves) {
    std::string test =
    "p td 9 9\n"
    "1 2\n"
    "2 3\n"
    "3 1\n"
    "1 4\n"
    "1 5\n"
    "2 6\n"
    "2 7\n"
    "3 8\n"
    "3 9\n";

    assertSurplus(-3, inputGraph(test));
}

TEST(TestSurplus, pentagramic) {
    std::string test =
    "p td 8 19\n"
    "1 2\n"
    "1 3\n"
    "1 4\n"
    "2 3\n"
    "3 4\n"
    "2 5\n"
    "5 6\n"
    "6 7\n"
    "7 4\n"
    "2 6\n"
    "2 7\n"
    "4 5\n"
    "4 6\n"
    "5 7\n"
    "5 6\n"
    "6 7\n"
    "6 8\n"
    "5 8\n"
    "8 7\n";

    assertSurplus(2, inputGraph(test));
}

TEST(TestSurplus, dense) {
    Graph g(12);
    for (int i = 0; i < 2; i++)
        for (int j = 2; j < 6; j++)
            g.addEdge(i, j);
    for (int i = 2; i < 6; i++)
        for (int j = 6; j < 10; j++)
            g.addEdge(i, j);
    for (int i = 6; i < 12; i++)
        for (int j = i + 1; j < 12; j++)
            g.addEdge(i, j);

    assertSurplus(2, g);
}

TEST(TestSurplus, complete) {
    Graph g(6);
    for (int i = 0; i < 6; i++)
        for (int j = i + 1; j < 6; j++)
            g.addEdge(i, j);

    assertSurplus(4, g);
}
