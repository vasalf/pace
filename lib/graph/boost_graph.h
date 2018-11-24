#pragma once

#include <boost/graph/adjacency_list.hpp>

#include <iostream>

namespace PaceVC {
namespace Boost {

using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using GraphTraits = boost::graph_traits<Graph>;
using Vertex = GraphTraits::vertex_descriptor;
using VertexIterator = GraphTraits::vertex_iterator;

Graph readGraph(std::istream& is);

}
}
