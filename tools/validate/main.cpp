#include <graph/graph.h>
#include <reader/line_reader.h>
#include <solution/solution.h>

#include <CLI11/CLI11.hpp>

#include <fstream>

namespace {

PaceVC::Solution readSolution(std::istream& is) {
    PaceVC::LineReader reader(is);

    auto firstLine = reader.nextLine();
    char p;
    std::string desc;
    int n, m;

    firstLine >> p >> desc >> n >> m;

    if (desc != "vc")
        throw std::runtime_error("this is not a solution");

    PaceVC::Solution ret { std::vector<int>(m) };
    auto line = reader.nextLine();
    for (int i = 0; i < m; i++) {
        while (!(line >> ret.vertices[i])) {
            if (is.eof()) {
                throw std::runtime_error("not enough vertices");
            }
            line = reader.nextLine();
        }
        ret.vertices[i]--;
    }

    return ret;
}

}

int main(int argc, char **argv) {
    CLI::App app("VC solution validator");

    std::string graphPath;
    std::string solutionPath;

    app.add_option("-g,--graph", graphPath, "Path to the instance of VC problem")->required();
    app.add_option("-s,--solution", solutionPath, "Path to the solution of VC problem");

    try {
        CLI11_PARSE(app, argc, argv);

        if (graphPath == "") {
            throw std::runtime_error("graph is not provided");
        }

        std::ifstream graph_f(graphPath);
        PaceVC::Graph graph = PaceVC::readGraph(graph_f);

        PaceVC::Solution solution;
        if (solutionPath == "") {
            solution = readSolution(std::cin);
        } else {
            std::ifstream solution_f(solutionPath);
            solution = readSolution(solution_f);
        }

        auto p = PaceVC::validate(graph, solution);
        if (p.has_value()) {
            std::cerr << "uncovered edge: {" << p.value().first + 1 << "," << p.value().second + 1 << "}" << std::endl;
            return 1;
        }
    } catch(std::runtime_error e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
