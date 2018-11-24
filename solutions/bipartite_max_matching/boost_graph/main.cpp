#include <iostream>
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>

using namespace std;
using namespace boost;

using graph_t = adjacency_list<vecS, vecS, undirectedS>;

int main() {
    int n;
    cin >> n;

    graph_t g(n);
    int m;
    cin >> m;

    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        u--; v--;

        add_edge(u, v, g);
    }

    std::vector<graph_traits<graph_t>::vertex_descriptor> mate(n);
    edmonds_maximum_cardinality_matching(g, mate.data());

    cout << matching_size(g, mate.data()) << endl;
    graph_traits<graph_t>::vertex_iterator vi, vi_end;
    for (tie(vi, vi_end) = vertices(g); vi != vi_end; vi++) {
        if (mate[*vi] != graph_traits<graph_t>::null_vertex() && *vi < mate[*vi]) {
            cout << *vi + 1 << " " << mate[*vi] + 1 << endl;
        }
    }

    return 0;
}
