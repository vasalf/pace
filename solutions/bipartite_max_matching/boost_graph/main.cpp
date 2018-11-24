#include <graph/boost_graph.h>

#include <boost/graph/max_cardinality_matching.hpp>

#include <iostream>
#include <vector>

using namespace std;
using namespace boost;


int main() {
    PaceVC::Boost::Graph g = PaceVC::Boost::readGraph(std::cin);

    std::vector<PaceVC::Boost::Vertex> mate(num_vertices(g));
    edmonds_maximum_cardinality_matching(g, mate.data());

    cout << matching_size(g, mate.data()) << endl;
    return 0;
}
