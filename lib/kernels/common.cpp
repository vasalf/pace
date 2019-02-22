#include <kernels/common.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>

namespace PaceVC {
namespace Kernels {

void cleanUp(Graph& graph) {
    Graph::Set<int> undecidedCopy = graph.undecided();
    for (int u : undecidedCopy)
        if (graph.adjacent(u).empty())
            graph.removeVertex(u);
}

namespace {
    void dfs(const Graph& g, int v, std::function<bool (int)> allowed, std::map<int, int>& color, int c) {
        color[v] = c;
        for (int u : g.adjacent(v)) {
            if (!color.count(u) && allowed(u)) {
                dfs(g, u, allowed, color, c);
            }
        }
    }
}

void printGraphStats(const Graph& g, std::ostream& out) {
    int edges = 0;
    for (int i = 0; i < g.realSize(); i++) {
        edges += g.adjacent(i).size();
    }
    edges /= 2;

    std::vector<int> undecided;
    int setSize = std::min(g.size(), 10);
    std::copy(g.undecided().begin(), g.undecided().end(), std::back_inserter(undecided));
    std::nth_element(
        undecided.begin(),
        undecided.begin() + setSize,
        undecided.end(),
        [&g](int u, int v) { return g.adjacent(u).size() > g.adjacent(v).size(); }
    );

    std::map<int, int> color;
    int curc = 0;
    for (int i = 0; i < setSize; i++) {
        if (!color.count(undecided[i])) {
            dfs(g,
                undecided[i],
                [&undecided, setSize] (int v) -> bool { return std::find(undecided.begin(), undecided.begin() + setSize, v) != undecided.begin() + setSize; },
                color,
                curc
            );
            curc++;
        }
    }

    std::vector<std::vector<int> > comps(curc);
    for (int i = 0; i < setSize; i++) {
        comps[color[undecided[i]]].push_back(g.adjacent(undecided[i]).size());
    }

    for (auto& v: comps) {
        std::sort(v.rbegin(), v.rend());
    }
    std::sort(comps.rbegin(), comps.rend());

    out << "n: " << g.size() << std::endl;
    out << "m: " << edges << std::endl;
    out << "hi-deg: ";
    for (auto& v: comps) {
        out << "{";
        for (int i = 0; i < (int)v.size(); i++) {
            out << v[i];
            if (i + 1 != (int)v.size()) {
                out << " ";
            }
        }
        out << "} ";
    }
    out << std::endl;
}

}
}
