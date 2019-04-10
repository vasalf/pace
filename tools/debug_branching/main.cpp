#include <branching/cutpoints.h>
#include <branching/debug_branching.h>
#include <branching/max_degree.h>
#include <graph/graph.h>
#include <kernels/trivial.h>

#include <CLI11/CLI11.hpp>

#include <iostream>
#include <fstream>

using Solution = PaceVC::DebugBranching<PaceVC::Kernels::Trivial, PaceVC::CutpointsSelector<PaceVC::MaxDegreeSelector>>;

namespace {
    class StdoutLogger: public PaceVC::IDebugLogger {
    private:
        int curSpaces = 0;

        void writeSpaces() {
            for (int i = 0; i < curSpaces; i++) {
                std::cout << ' ';
            }
        }

    public:
        StdoutLogger()
        {}

        void onInnerVertex(int sizeAtStart, int reducedSize, int neigh) {
            writeSpaces();
            std::cout << "inner(start=" << sizeAtStart << ", reduced=" << reducedSize << ", neighbors=" << neigh << ")" << std::endl;
        }

        void onDisconnectedGraph(const std::vector<int>& sizes) {
            writeSpaces();
            std::cout << "disconnected(size={";
            for (int i = 0; i < (int)sizes.size(); i++) {
                std::cout << sizes[i];
                if (i != (int)sizes.size() - 1)
                    std::cout << ",";
            }
            std::cout << "})" << std::endl;
        }

        void onBoundLeaf(int finalSize) {
            writeSpaces();
            std::cout << "leaf(finalSize=" << finalSize << ")" << std::endl;
        }

        void onSolution(int solutionSize) {
            writeSpaces();
            std::cout << "solution(size=" << solutionSize << ")" << std::endl;
        }

        void toChild() {
            curSpaces++;
        }

        void retFromChild() {
            curSpaces--;
        }
    };
}

int main(int argc, char* argv[]) {
    CLI::App app("Debug branching");

    std::string graphPath;
    int maxDepth = -1;

    app.add_option("-g,--graph", graphPath, "Path to the instance of VC problem")->required();
    app.add_option("-d,--depth", maxDepth, "Maximum depth of the trace");

    try {
        CLI11_PARSE(app, argc, argv);

        if (graphPath == "") {
            throw std::runtime_error("graph is not provided");
        }
    } catch(std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    std::ifstream graphFile(graphPath);
    PaceVC::Graph graph = PaceVC::readGraph(graphFile);

    std::shared_ptr<StdoutLogger> logger = std::make_shared<StdoutLogger>();
    Solution(graph).branch(logger, maxDepth);

    std::cout << "Bounded branching best solution: " << graph.bestSolution().size() << std::endl;

    return 0;
}
