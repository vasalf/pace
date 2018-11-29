#include <gtest/gtest.h>

#include <bipartite_maxm/min_vc.h>

#include <bipartite_maxm/augmenting_path.h>
#include <bipartite_maxm/matching.h>
#include <graph/graph.h>

using namespace PaceVC;

namespace {

bool checkVC(const BipartiteGraph& g, std::vector<BipartiteGraph::Vertex> vc) {
    std::vector<bool> leftVC(g.leftSize()), rightVC(g.rightSize());
    for (auto p : vc) {
        if (p.first == BipartiteGraph::Part::LEFT)
            leftVC[p.second] = true;
        else
            rightVC[p.second] = true;
    }

    for (int i = 0; i < g.leftSize(); i++)
        for (int u : g.neighboursOfLeft(i))
            if (!leftVC[i] && !rightVC[u])
                return false;

    return true;
}

template<class MaxMFinder, class AugPathFinder>
struct TestCase {
    using VCFinder = MinVCFinder<MaxMFinder, AugPathFinder>;
};

}

template<class MinVCFinder>
struct TestVC : public testing::Test {
    using VCFinder = typename MinVCFinder::VCFinder;
};

using VCFinders = ::testing::Types<
    TestCase<ClassicKuhnMatchingFinder<KuhnAugmentingPathFinder>, KuhnAugmentingPathFinder>,
    TestCase<ClassicKuhnMatchingFinder<OptimizedKuhnAugmentingPathFinder>, KuhnAugmentingPathFinder>,
    TestCase<OptimizedKuhnMatchingFinder<KuhnAugmentingPathFinder>, KuhnAugmentingPathFinder>,
    TestCase<OptimizedKuhnMatchingFinder<OptimizedKuhnAugmentingPathFinder>, KuhnAugmentingPathFinder>,
    TestCase<HopcroftKarpMatchingFinder, KuhnAugmentingPathFinder>
>;
TYPED_TEST_CASE(TestVC, VCFinders);


TYPED_TEST(TestVC, dummy) {
    using VCFinder = typename TestFixture::VCFinder;

    BipartiteGraph g(1, 1);

    {
        VCFinder vc(g);
        ASSERT_EQ(0, vc.find());
        ASSERT_TRUE(checkVC(g, vc.answer));
    }

    g.addEdge(0, 0);

    {
        VCFinder vc(g);
        ASSERT_EQ(1, vc.find());
        ASSERT_TRUE(checkVC(g, vc.answer));
    }
}

TYPED_TEST(TestVC, smoke) {
    using VCFinder = typename TestFixture::VCFinder;

    BipartiteGraph g(5, 5);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(0, 3);
    g.addEdge(1, 0);
    g.addEdge(2, 0);
    g.addEdge(3, 0);
    g.addEdge(4, 0);
    g.addEdge(0, 0);

    {
        VCFinder vc(g);
        ASSERT_EQ(2, vc.find());
        ASSERT_TRUE(checkVC(g, vc.answer));
    }

    g.addEdge(4, 4);
    g.addEdge(3, 3);

    {
        VCFinder vc(g);
        ASSERT_EQ(4, vc.find());
        ASSERT_TRUE(checkVC(g, vc.answer));
    }
}


