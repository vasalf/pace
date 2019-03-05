#pragma once

#ifdef USE_ABSL_HASH_TABLES
#include <absl/container/flat_hash_set.h>
#endif

#include <memory>
#include <vector>
#include <unordered_set>

namespace PaceVC {

class Graph {
    struct ReductionBase {
        virtual void reduce(Graph&, const Graph&) = 0;
    };

    using ReductionRulePtr = std::shared_ptr<ReductionBase>;

    template<class ReductionRule>
    struct ReductionImpl : public ReductionBase {
        ReductionRule rule;

        ReductionImpl(ReductionRule&& ofRule) : rule(ofRule) {}

        virtual void reduce(Graph& oldGraph, const Graph& nextStepRes) override final {
            rule(oldGraph, nextStepRes);
        }
    };

public:
#ifdef USE_ABSL_HASH_TABLES
    template<class T>
    using Set = absl::flat_hash_set<int>;
#else
    template<class T>
    using Set = std::unordered_set<int>;
#endif

    Graph(int n);
    ~Graph();

    Graph(const Graph&);
    Graph& operator=(const Graph&);
    Graph(Graph&&);
    Graph& operator=(Graph&&);

    void addEdge(int u, int v);
    const Set<int>& adjacent(int v) const;

    void takeVertex(int v);
    void removeVertex(int v);
    void saveSolution(std::vector<int> solution);
    std::vector<int> bestSolution() const;
    const Set<int>& undecided() const;
    const std::vector<int>& solution() const;
    const std::vector<int>& removed() const;

    template<class ReductionRule>
    void addReduction(ReductionRule&& rule, const Graph& newGraph) {
        addReductionImpl(
            std::make_shared<ReductionImpl<ReductionRule>>(std::forward<ReductionRule>(rule)),
            newGraph
        );
    }

    void addSimpleReduction(const std::vector<int>& newToOld);
    void trySqueeze();
    void span(const std::vector<int>& toSpan, const std::vector<int>& ifTook, const std::vector<int>& ifNotTook);

    std::vector<int> restoreSolution();

    int size() const;
    int realSize() const;
    int sizeOnCreation() const;

    void placeMark();
    void restoreMark();

private:
    void addReductionImpl(ReductionRulePtr rule, const Graph& newGraph);

#ifdef GRAPH_UT
public:
#endif
    void squeeze();

private:
    struct TImpl;
    std::unique_ptr<TImpl> impl_;

    Graph(std::unique_ptr<TImpl>&&);
};

Graph readGraph(std::istream& is);
void printSolution(std::ostream& os, Graph& g);

}
