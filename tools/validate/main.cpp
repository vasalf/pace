#include <graph/graph.h>
#include <reader/line_reader.h>

#include <gflags/gflags.h>

#include <fstream>

namespace {

struct Solution {
    std::vector<int> vertices;
};

Solution readSolution(std::istream& is) {
    PaceVC::LineReader reader(is);

    auto firstLine = reader.nextLine();
    char p;
    std::string desc;
    int n, m;

    firstLine >> p >> desc >> n >> m;

    if (desc != "vc")
        throw std::runtime_error("this is not a solution");

    Solution ret { std::vector<int>(m) };
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

bool validate(const PaceVC::Graph& g, const Solution& s) {
    std::vector<bool> inSolution(g.realSize());
    for (int u : s.vertices) {
        inSolution[u] = true;
    }

    for (int i = 0; i < g.realSize(); i++) {
        for (int v : g.adjacent(i)) {
            if (!inSolution[i] && !inSolution[v]) {
                std::cerr << "uncovered edge {" << i + 1 << "," << v + 1 << "}" << std::endl;
                return false;
            }
        }
    }
    return true;
}

}

DEFINE_string(graph, "", "Path to the instance of VC problem");
DEFINE_string(solution, "", "Path to the solution of VC problem");

int main(int argc, char **argv) {
    try {
        google::ParseCommandLineFlags(&argc, &argv, true);

        if (FLAGS_graph == "") {
            throw std::runtime_error("graph is not provided");
        }

        std::ifstream graph_f(FLAGS_graph);
        PaceVC::Graph graph = PaceVC::readGraph(graph_f);

        Solution solution;
        if (FLAGS_solution == "") {
            solution = readSolution(std::cin);
        } else {
            std::ifstream solution_f(FLAGS_solution);
            Solution solution = readSolution(solution_f);
        }

        if (!validate(graph, solution)) {
            return 1;
        }
    } catch(std::runtime_error e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
