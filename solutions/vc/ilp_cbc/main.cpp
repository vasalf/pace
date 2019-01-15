#include <graph/graph.h>

#define USE_CBC
#include <ortools/linear_solver/linear_solver.h>

#include <iostream>

using namespace operations_research;

int main() {
    PaceVC::Graph graph = PaceVC::readGraph(std::cin);

    MPSolver solver("problem", MPSolver::CBC_MIXED_INTEGER_PROGRAMMING);

    std::vector<MPVariable*> vars;
    for (int i = 0; i < graph.realSize(); i++) {
        vars.push_back(solver.MakeIntVar(0, 0, ""));
    }

    for (int u : graph.undecided()) {
        for (int v : graph.adjacent(u)) {
            if (u < v) {
                std::cout << u << " " << v << std::endl;
                LinearRange edgeVal = LinearExpr(vars[u]) + LinearExpr(vars[v]) >= 1;
                solver.MakeRowConstraint(edgeVal);
            }
        }
    }

    auto objective = solver.MutableObjective();
    for (int u : graph.undecided()) {
        objective->SetCoefficient(vars[u], 1);
    }
    objective->SetMinimization();

    solver.Solve();

    std::vector<int> solution = graph.solution();
    for (int u : graph.undecided()) {
        if (vars[u]->solution_value()) {
            solution.push_back(u);
        }
    }
    graph.saveSolution(solution);

    PaceVC::printSolution(std::cout, graph);

    return 0;
}
