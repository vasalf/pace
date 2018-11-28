#include <gtest/gtest.h>

#include <bipartite_maxm/matching.h>

#include <bipartite_maxm/augmenting_path.h>
#include <graph/bipartite_graph.h>

using namespace PaceVC;

template<class MaxMFinder>
struct TestMatching : public testing::Test {
    using Finder = MaxMFinder;
};

using MatchingFinders = ::testing::Types<
    ClassicKuhnMatchingFinder<KuhnAugmentingPathFinder>,
    ClassicKuhnMatchingFinder<OptimizedKuhnAugmentingPathFinder>,
    OptimizedKuhnMatchingFinder<KuhnAugmentingPathFinder>,
    OptimizedKuhnMatchingFinder<OptimizedKuhnAugmentingPathFinder>,
    HopcroftKarpMatchingFinder
>;
TYPED_TEST_CASE(TestMatching, MatchingFinders);


TYPED_TEST(TestMatching, dummy) {
    using MaxMFinder = typename TestFixture::Finder;

    BipartiteGraph g(1, 1);
    ASSERT_EQ(0, MaxMFinder(g).find());

    g.addEdge(0, 0);
    ASSERT_EQ(1, MaxMFinder(g).find());
}

TYPED_TEST(TestMatching, smoke) {
    using MaxMFinder = typename TestFixture::Finder;

    BipartiteGraph g(5, 5);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(0, 3);
    g.addEdge(1, 0);
    g.addEdge(2, 0);
    g.addEdge(3, 0);
    g.addEdge(4, 0);
    g.addEdge(0, 0);

    ASSERT_EQ(2, MaxMFinder(g).find());

    g.addEdge(4, 4);
    g.addEdge(3, 3);

    ASSERT_EQ(4, MaxMFinder(g).find());
}

