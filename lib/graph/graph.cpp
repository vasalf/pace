#include <graph/graph.h>
#include <reader/line_reader.h>

#include <algorithm>
#include <cassert>
#include <numeric>

namespace {

static int numberOfExistingGraphImpls = 0;

struct GraphImpl {
    int size;
    std::vector<PaceVC::Graph::Set<int>> graph;
    PaceVC::Graph::Set<int> undecided;
    std::vector<int> solution;
    std::vector<int> removed;

    GraphImpl() = default;
    ~GraphImpl() = default;

    GraphImpl(int n) {
        size = n;
        graph.resize(n);

        for (int i = 0; i < n; i++)
            undecided.insert(i);
    }

    GraphImpl(const GraphImpl&) = default;
    GraphImpl& operator=(const GraphImpl&) = default;
    GraphImpl(GraphImpl&&) = default;
    GraphImpl& operator=(GraphImpl&&) = default;

    void addEdge(int u, int v) {
        assert(u != v);

        if (!undecided.count(u) || !undecided.count(v)) {
            return;
        }

        graph[u].insert(v);
        graph[v].insert(u);
    }

    void decideVertex(int v) {
        undecided.erase(v);

        for (int u : graph[v])
            graph[u].erase(v);
    }

    void takeVertex(int v) {
        if (!undecided.count(v))
            throw std::runtime_error("Attempt to take a decided vertex");

        solution.push_back(v);
        decideVertex(v);
    }

    void removeVertex(int v) {
        if (!undecided.count(v))
            throw std::runtime_error("Attempt to remove a decided vertex");

        removed.push_back(v);
        decideVertex(v);
    }
};

struct GraphMark {
    GraphImpl impl;
    int implStackSize;
    int reductionStackSize;
};

}

namespace PaceVC {

struct Graph::TImpl {
    std::vector<GraphImpl> implStack;
    std::vector<ReductionRulePtr> reductionStack;
    std::vector<int> bestSolution;
    std::vector<GraphMark> marks;

    TImpl(int n) {
        implStack.push_back(GraphImpl(n));
        bestSolution.resize(n);
        std::iota(bestSolution.begin(), bestSolution.end(), 0);
    }

    TImpl(const GraphImpl& impl) {
        implStack.push_back(impl);
        bestSolution.resize(impl.graph.size());
        std::iota(bestSolution.begin(), bestSolution.end(), 0);
    }

    TImpl(const TImpl&) = default;
    TImpl& operator=(const TImpl&) = default;
};

Graph::Graph(int n) : impl_(std::make_unique<TImpl>(n)) {}

Graph::~Graph() = default;

Graph::Graph(const Graph& g) : impl_(std::make_unique<TImpl>(*g.impl_)) {}

Graph& Graph::operator=(const Graph& g) {
    impl_ = std::make_unique<TImpl>(*g.impl_);
    return *this;
}

Graph::Graph(Graph&& g) : impl_(std::move(g.impl_)) {}

Graph& Graph::operator=(Graph&& g) {
    impl_ = std::move(g.impl_);
    return *this;
}

void Graph::addEdge(int u, int v) {
    if (u == v) {
        if (undecided().count(u)) {
            takeVertex(u);
        }
    } else {
        impl_->implStack.back().addEdge(u, v);
    }
}

const Graph::Set<int>& Graph::adjacent(int v) const {
    return impl_->implStack.back().graph[v];
}

void Graph::takeVertex(int v) {
    impl_->implStack.back().takeVertex(v);
}

void Graph::removeVertex(int v) {
    impl_->implStack.back().removeVertex(v);
}

void Graph::saveSolution(std::vector<int> solution) {
    if (impl_->bestSolution.size() > solution.size()) {
        impl_->bestSolution = solution;
    }
}

std::vector<int> Graph::bestSolution() const {
    return impl_->bestSolution;
}

const Graph::Set<int>& Graph::undecided() const {
    return impl_->implStack.back().undecided;
}

const std::vector<int>& Graph::solution() const {
    return impl_->implStack.back().solution;
}

const std::vector<int>& Graph::removed() const {
    return impl_->implStack.back().removed;
}

void Graph::addSimpleReduction(const std::vector<int>& newToOld) {
    std::vector<int> oldToNew(realSize(), -1);
    for (size_t i = 0; i != newToOld.size(); i++) {
        oldToNew[newToOld[i]] = i;
    }

    GraphImpl squeezed(newToOld.size());
    for (size_t i = 0; i != newToOld.size(); i++) {
        for (int u : adjacent(newToOld[i])) {
            if (oldToNew[u] != -1)
                squeezed.addEdge(i, oldToNew[u]);
        }
    }

    addReduction([newToOld](Graph& oldGraph, const Graph& nextStepRes) {
        for (int u : nextStepRes.solution())
            if (oldGraph.undecided().count(newToOld[u]))
                oldGraph.takeVertex(newToOld[u]);
        for (int u : nextStepRes.removed())
            if (oldGraph.undecided().count(newToOld[u]))
                oldGraph.removeVertex(newToOld[u]);
    }, Graph(std::make_unique<TImpl>(squeezed)));
}

void Graph::squeeze() {
    std::vector<int> newToOld;
    std::copy(undecided().begin(), undecided().end(), std::back_inserter(newToOld));
    addSimpleReduction(newToOld);
}

void Graph::restoreSolution() {
    while (impl_->implStack.size() > 1) {
        auto oldGraphImpl = std::make_unique<Graph::TImpl>(impl_->implStack[impl_->implStack.size() - 2]);
        Graph oldGraph(std::move(oldGraphImpl));

        auto newGraphImpl = std::make_unique<Graph::TImpl>(impl_->implStack.back());
        Graph newGraph(std::move(newGraphImpl));

        impl_->reductionStack.back()->reduce(oldGraph, newGraph);

        impl_->implStack.pop_back();
        impl_->reductionStack.pop_back();
        impl_->implStack.back() = oldGraph.impl_->implStack.back();
    }
}

int Graph::size() const {
    return undecided().size();
}

int Graph::realSize() const {
    return impl_->implStack.back().size;
}

void Graph::placeMark() {
    impl_->marks.push_back({impl_->implStack.back(), impl_->implStack.size(), impl_->reductionStack.size()});
}

void Graph::restoreMark() {
    if (impl_->marks.empty()) {
        throw std::runtime_error("Attempt to restore mark in graph with empty mark stack");
    }

    impl_->implStack.resize(impl_->marks.back().implStackSize);
    impl_->reductionStack.resize(impl_->marks.back().reductionStackSize);
    impl_->implStack.back() = std::move(impl_->marks.back().impl);
    impl_->marks.pop_back();
}

void Graph::addReductionImpl(ReductionRulePtr rule, const Graph& newGraph) {
    impl_->implStack.push_back(newGraph.impl_->implStack.back());
    impl_->reductionStack.push_back(rule);
}

Graph::Graph(std::unique_ptr<TImpl>&& impl) : impl_(std::move(impl)) {}

Graph readGraph(std::istream& is) {
    LineReader reader(is);

    auto firstLine = reader.nextLine();
    char p;
    std::string desc;
    int n, m;
    firstLine >> p >> desc >> n >> m;

    if (desc != "td")
        throw std::runtime_error("this is not a graph");

    Graph ret(n);
    for (int i = 0; i < m; i++) {
        auto line = reader.nextLine();
        int u, v;
        line >> u >> v;
        u--; v--;
        ret.addEdge(u, v);
    }

    return ret;
}

void printSolution(std::ostream& os, Graph& graph) {
    graph.restoreSolution();

    os << "c What are those comments for?" << std::endl
       << "s vc " << graph.realSize() << " " << graph.bestSolution().size() << std::endl;

    for (int u : graph.bestSolution())
        os << u + 1 << std::endl;
}

}
