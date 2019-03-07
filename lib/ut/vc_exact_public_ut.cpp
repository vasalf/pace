#include <gtest/gtest.h>

#include <branching/cutpoints.h>
#include <graph/graph.h>
#include <graph/util.h>
#include <solution/greed.h>
#include <solution/solution.h>

#include <filesystem>
#include <fstream>
#include <sstream>

using namespace PaceVC;

class TestVCPublic : public ::testing::TestWithParam<int> {
protected:
    std::string testFilename() {
        namespace fs = std::filesystem;
        std::ostringstream filename;
        filename << "vc-exact_";
        filename << std::setfill('0') << std::setw(3) << GetParam();
        filename << ".hgr";
        std::string source_dir = SOURCE_DIR;
        return fs::path(source_dir) / "data/vc-exact-public" / filename.str();
    }

    Graph testGraph() {
        std::ifstream fin(testFilename());
        return readGraph(fin);
    }
};

INSTANTIATE_TEST_CASE_P(TestVCExactPublic, TestVCPublic, ::testing::Range(1, 200, 2));

TEST_P(TestVCPublic, testGreedCoversAll) {
    Graph g = testGraph();
    Graph h = g;
    saveGreedSolution(g);
    ASSERT_FALSE(validate(h, {g.bestSolution()}).has_value());
}

/*
TEST_P(TestVCPublic, testCutpointsSplitIntoComponents) {
    Graph g = testGraph();
    auto cuts = cutpoints(g);
    for (int u : cuts) {
        g.placeMark();
        g.takeVertex(u);
        ASSERT_TRUE(connectedComponents(g).size() > 1);
        g.restoreMark();
    }
}
*/

TEST_P(TestVCPublic, testCutpointsBranchingWithTrivialKernel) {
    Graph g = testGraph();
    Graph h = g;
    CutpointsBranching<Kernels::Trivial>(g).branch();
    ASSERT_FALSE(validate(h, {g.bestSolution()}).has_value());
}
