#include <kernels/lp/glop.h>

#define USE_GLOP
#include <ortools/linear_solver/linear_solver.h>
#include <ortools/linear_solver/linear_solver.pb.h>

#include <iostream>
#include <sstream>
#include <vector>

namespace {

void reduceImpl(PaceVC::Graph& g, operations_research::MPSolver::OptimizationProblemType pt) {
    using namespace operations_research;

    MPSolver solver("Glop", pt);

    std::vector<MPVariable*> vars;
    solver.MakeVarArray(g.size(), 0, 1, false, "v", &vars);

    for (int u = 0; u < g.size(); u++) {
        for (int v : g.adjacent(u)) {
            if (u < v) {
                LinearRange edgeVal = LinearExpr(vars[u]) + LinearExpr(vars[v]) >= 1;
                solver.MakeRowConstraint(edgeVal);
            }
        }
    }

    MPObjective* objective = solver.MutableObjective();
    for (int u = 0; u < g.size(); u++) {
        objective->SetCoefficient(vars[u], 1);
    }
    objective->SetMinimization();

    solver.Solve();

    for (int u = 0; u < g.size(); u++) {
        const double value = vars[u]->solution_value();
        std::cout << value;
        if (value < 0.5) {
            g.removeVertex(u);
            std::cout << " remove" << std::endl;
        } else if (value > 0.5) {
            g.takeVertex(u);
            std::cout << " take" << std::endl;
        }
    }
}

}

namespace PaceVC {
namespace Kernels {

GlopLPKernel::GlopLPKernel(Graph& g)
    : graph(g)
{}

void GlopLPKernel::reduce() {
    graph.squeeze();
    reduceImpl(graph, operations_research::MPSolver::GLOP_LINEAR_PROGRAMMING);
}

}
}