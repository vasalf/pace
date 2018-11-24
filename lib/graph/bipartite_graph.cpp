#include <graph/bipartite_graph.h>

#include <reader/line_reader.h>

namespace PaceVC {

BipartiteGraph::BipartiteGraph(int n, int k)
    : n_(n), k_(k), leftNbs_(n), rightNbs_(k) {}

void BipartiteGraph::addEdge(int u, int v) {
    leftNbs_[u].push_back(v);
    rightNbs_[v].push_back(v);
}

const std::vector<int>& BipartiteGraph::neighboursOfLeft(int u) const {
    return leftNbs_[u];
}

const std::vector<int>& BipartiteGraph::neighboursOfRight(int v) const {
    return rightNbs_[v];
}

int BipartiteGraph::leftSize() const {
    return n_;
}

int BipartiteGraph::rightSize() const {
    return k_;
}

BipartiteGraph readBipartiteGraph(std::istream& is) {
    LineReader reader(is);

    auto firstLine = reader.nextLine();
    char p;
    std::string desc;
    int n, k, m;
    firstLine >> p >> desc >> n >> k >> m;

    if (desc != "bi")
        throw std::runtime_error("this is not a bipartite graph");

    BipartiteGraph ret(n, k);
    for (int i = 0; i < m; i++) {
        auto line = reader.nextLine();
        int u, v;
        line >> u >> v;
        u--; v--;
        ret.addEdge(u, v);
    }

    return ret;
}

}
