#include <graph/graph.h>

#include <ortools/sat/cp_model.h>

#include <iostream>

using namespace operations_research;
using namespace operations_research::sat;

int main() {
    PaceVC::Graph graph = PaceVC::readGraph(std::cin);

    CpModelBuilder model;
    Domain domain(0, 1);

    std::vector<IntVar> vars;
    for (int i = 0; i < graph.realSize(); i++) {
        vars.push_back(model.NewIntVar(domain));
    }

    Domain edgeDomain(1, graph.realSize());

    for (int u : graph.undecided()) {
        for (int v : graph.adjacent(u)) {
            if (u < v) {
                LinearExpr expr;
                expr.AddVar(vars[u]);
                expr.AddVar(vars[v]);
                model.AddLinearConstraint(expr, edgeDomain);
            }
        }
    }

    LinearExpr objective;
    for (auto& v: vars)
        objective.AddVar(v);

    model.Minimize(objective);

    auto solution = Solve(model);

    std::vector<int> vsolution = graph.solution();
    for (int u : graph.undecided()) {
        if (SolutionIntegerValue(solution, vars[u])) {
            vsolution.push_back(u);
        }
    }

    graph.saveSolution(vsolution);
    PaceVC::printSolution(std::cout, graph);

    return 0;
}
