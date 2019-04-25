#include <cut/vertex_cut.h>

#include <graph/util.h>

#include <ortools/graph/max_flow.h>

#include <cassert>
#include <random>

namespace PaceVC {
namespace {

std::vector<int> verticesForChoose(const Graph& graph, const std::vector<int>& component) {
    std::vector<int> ret;
    for (int u : component) {
        if (graph.adjacent(u).size() > 3)
            ret.push_back(u);
    }
    if (ret.size() < 2) {
        return component;
    }
    return ret;
}

std::vector<int> cutInComponent(const Graph& graph, std::vector<int> component, std::mt19937& rnd) {
    std::vector<int> choose = verticesForChoose(graph, component);
    std::uniform_int_distribution<int> dist(0, component.size() - 1);
    std::vector<int> optAnswer;
    int edges = 0;
    for (int u : component) {
        edges += graph.adjacent(u).size();
    }
    if (edges == (long long)(1ll * component.size() * (component.size() - 1))) {
        return component;
    }
    for (int t = 0; t < 5; t++) {
        std::vector<int> arcIds;
        operations_research::SimpleMaxFlow maxFlow;
        for (int u : component) {
            int id = maxFlow.AddArcWithCapacity(2 * u, 2 * u + 1, 1);
            arcIds.push_back(id);
            for (int v : graph.adjacent(u)) {
                maxFlow.AddArcWithCapacity(2 * u + 1, 2 * v, component.size() + 1);
            }
        }
        int u, v;
        do {
            u = dist(rnd);
            v = dist(rnd);
        } while (u == v || graph.adjacent(component[u]).count(component[v]));
        u = component[u];
        v = component[v];
        auto status = maxFlow.Solve(2 * u + 1, 2 * v);
        assert(status == operations_research::SimpleMaxFlow::Status::OPTIMAL);
        if (optAnswer.empty() || maxFlow.OptimalFlow() < (int)optAnswer.size()) {
            optAnswer.clear();
            std::vector<int> a;
            maxFlow.GetSourceSideMinCut(&a);
            std::sort(a.begin(), a.end());
            for (int u : component) {
                if (std::binary_search(a.begin(), a.end(), 2 * u)
                    && !std::binary_search(a.begin(), a.end(), 2 * u + 1)) {
                    optAnswer.push_back(u);
                }
            }
            assert((int)optAnswer.size() == maxFlow.OptimalFlow());
        }
    }
    return optAnswer;
}

}

VertexCutFinder::VertexCutFinder(Graph& graph)
    : g(graph)
{}

int VertexCutFinder::find() {
    std::mt19937 rnd(179);
    for (auto component : connectedComponents(g)) {
        std::vector<int> candidate = cutInComponent(g, component, rnd);
        if (answer.empty() || answer.size() > candidate.size()) {
            answer = candidate;
        }
    }
    return answer.size();
}

}
