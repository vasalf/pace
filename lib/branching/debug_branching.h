#pragma once

#include <graph/graph.h>
#include <graph/util.h>
#include <kernels/bound.h>
#include <kernels/trivial.h>

#include <algorithm>
#include <vector>

namespace PaceVC {

class IDebugLogger {
public:
    virtual void onInnerVertex(int sizeAtStart, int reducedSize, int neigh) = 0;
    virtual void onDisconnectedGraph(const std::vector<int>& sizes) = 0;
    virtual void onBoundLeaf(int finalSize) = 0;
    virtual void onSolution(int solutionSize) = 0;
    virtual void toChild() = 0;
    virtual void retFromChild() = 0;
};

using DebugLoggerPtr = std::shared_ptr<IDebugLogger>;

class EmptyLogger final: public IDebugLogger {
    void onInnerVertex(int, int, int) override;
    void onDisconnectedGraph(const std::vector<int>&) override;
    void onBoundLeaf(int) override;
    void onSolution(int) override;
    void toChild() override;
    void retFromChild() override;
};

template<class IntermediateReducer, class VertexSelector>
class DebugBranching {
    using TThis = DebugBranching<IntermediateReducer, VertexSelector>;

public:

    DebugBranching(Graph& g)
        : graph(g)
    {}

    void branch(DebugLoggerPtr logger, int heightBound) {
        graph.placeMark();
        doBranch(graph.size(), heightBound, logger);
        graph.restoreMark();
    }

private:
    Graph& graph;
    int currentSpans = 0;

    void doBranch(int bound, int heightBound, DebugLoggerPtr logger) {
        graph.trySqueeze();

        int sizeAtStart = graph.size();

        int lowerBound = 0;

        if (bound >= graph.size() || bound < 0) {
            IntermediateReducer k(graph);
            k.reduce();
            bound = Kernels::getKernelBound(k);
            currentSpans += Kernels::getKernelSpans(k);
            lowerBound = graph.restoreSolution().size() + Kernels::getKernelLowerBound(k) + currentSpans;
        }

        Kernels::Trivial trivial(graph);
        trivial.reduce();
        currentSpans += Kernels::getKernelSpans(trivial);

        int reducedSize = graph.size();

        if (graph.size() == 0) {
            logger->onSolution(graph.restoreSolution().size());
            graph.saveSolution(graph.restoreSolution());
            return;
        }

        if (std::max<int>(graph.restoreSolution().size(), lowerBound) >= graph.bestSolution().size()) {
            return;
        }

        if (heightBound == 0) {
            logger->onBoundLeaf(reducedSize);
            return;
        }

        auto components = connectedComponents(graph);
        if (components.size() > 1) {
            std::vector<int> componentSizes(components.size());
            std::transform(components.begin(), components.end(), componentSizes.begin(),
                           [](const std::vector<int>& v) { return v.size(); });
            logger->onDisconnectedGraph(componentSizes);

            std::vector<int> indexInComponent(graph.realSize());
            for (int i = 0; i < (int)components.size(); i++) {
                for (int j = 0; j < (int)components[i].size(); j++) {
                    indexInComponent[components[i][j]] = j;
                }
            }

            for (const auto& component : components) {
                Graph g(component.size());
                for (int u : component) {
                    for (int v : graph.adjacent(u)) {
                        g.addEdge(indexInComponent[u], indexInComponent[v]);
                    }
                }

                TThis(g).branch(std::make_shared<EmptyLogger>(), -1);

                for (int u : g.bestSolution()) {
                    graph.takeVertex(component[u]);
                }
                for (int u : component) {
                    if (graph.undecided().count(u))
                        graph.removeVertex(u);
                }
            }

            graph.saveSolution(graph.restoreSolution());
            return;
        }

        int v = VertexSelector(graph).select();

        logger->onInnerVertex(sizeAtStart, reducedSize, graph.adjacent(v).size());

        graph.placeMark();
        graph.takeVertex(v);
        logger->toChild();
        int oldSpans = currentSpans;
        doBranch(bound, heightBound - 1, logger);
        currentSpans = oldSpans;
        logger->retFromChild();
        graph.restoreMark();

        graph.placeMark();
        auto adjCopy = graph.adjacent(v);
        for (int u : adjCopy) {
            graph.takeVertex(u);
        }
        logger->toChild();
        oldSpans = currentSpans;
        doBranch(bound, heightBound - 1, logger);
        currentSpans = oldSpans;
        logger->retFromChild();
        graph.restoreMark();
    }
};

}
