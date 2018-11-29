#pragma once

#include <iostream>
#include <vector>

namespace PaceVC {

/**
 * Vertices in each part are numbered from 0 to |part| - 1
 */
class BipartiteGraph {
public:
    enum class Part {
        LEFT, RIGHT
    };

    using Vertex = std::pair<Part, int>;

    BipartiteGraph(int n, int k);

    void addEdge(int u, int v);

    const std::vector<int>& neighboursOfLeft(int u) const;
    const std::vector<int>& neighboursOfRight(int v) const;

    int leftSize() const;
    int rightSize() const;

private:
    int n_, k_;
    std::vector<std::vector<int> > leftNbs_, rightNbs_;
};

BipartiteGraph readBipartiteGraph(std::istream& is);

}
