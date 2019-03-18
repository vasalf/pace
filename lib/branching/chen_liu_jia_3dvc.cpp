#include <branching/chen_liu_jia_3dvc.h>

#include <graph/util.h>
#include <kernels/trivial.h>

namespace PaceVC {

ChenLiuJia3DVC::ChenLiuJia3DVC(Graph& graph)
    : g(graph)
{}

void ChenLiuJia3DVC::fold(int v) {
    if (g.adjacent(v).size() != 2)
        throw std::runtime_error("Attempt to fold vertex of degree not equal to 2");
    int u, w;
    u = *g.adjacent(v).begin();
    w = *std::next(g.adjacent(v).begin());
    g.span({u, v, w}, {u, w}, {v});
}

void ChenLiuJia3DVC::branchOn(int v) {
    g.placeMark();
    g.takeVertex(v);
    branch();
    g.restoreMark();

    g.placeMark();
    for (int u : g.adjacent(v))
        g.takeVertex(u);
    g.removeVertex(v);
    branch();
    g.restoreMark();
}

void ChenLiuJia3DVC::doBranch() {
    Kernels::handleIsolated(g);

    auto components = connectedComponents(g);
    if (components.size() > 1) {
        std::vector<int> indexInComponent(g.realSize());
        for (int i = 0; i < (int)components.size(); i++) {
            for (int j = 0; j < (int)components.size(); j++) {
                indexInComponent[components[i][j]] = j;
            }
        }

        for (const auto& component : components) {
            Graph h(component.size());
            for (int u : component) {
                for (int v : g.adjacent(u)) {
                    h.addEdge(indexInComponent[u], indexInComponent[v]);
                }
            }

            TThis(h).branch();

            for (int u : h.bestSolution()) {
                h.takeVertex(component[u]);
            }
            for (int u : component) {
                if (g.undecided().count(u))
                    g.removeVertex(u);
            }
        }

        g.saveSolution(g.restoreSolution());
        return;
    }

    Kernels::handleLeaves(g);

    if (g.size() == 0) {
        g.saveSolution(g.restoreSolution());
        return;
    }

    g.trySqueeze();

    for (int u : g.undecided()) {
        if (g.adjacent(u).size() > 3) {
            branchOn(u);
            return;
        }
    }

    for (int u : g.undecided()) {
        if (g.adjacent(u).size() == 2) {
            fold(u);
            int nv = g.realSize() - 1;
            if (g.adjacent(nv).size() == 4) {
                branchOn(nv);
                return;
            }
        }
    }

    Kernels::Trivial(g).reduce();

    if (g.size() == 0) {
        g.saveSolution(g.restoreSolution());
        return;
    }

    branchOn(*g.undecided().begin());
}

void ChenLiuJia3DVC::branch() {
    g.placeMark();
    doBranch();
    g.restoreMark();
}

}
