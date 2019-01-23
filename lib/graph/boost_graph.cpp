#include <graph/boost_graph.h>
#include <reader/line_reader.h>

namespace PaceVC {
namespace Boost {
    Graph readGraph(std::istream& is) {
        LineReader reader(is);

        auto firstLine = reader.nextLine();
        char p; firstLine >> p;
        std::string descriptor; firstLine >> descriptor;

        if (descriptor == "bi") {
            int n, k, m;
            firstLine >> n >> k >> m;

            Graph g(n + k);

            for (int i = 0; i < m; i++) {
                auto line = reader.nextLine();
                int u, v;
                line >> u >> v;
                u--; v--;
                boost::add_edge(u, v + n, g);
            }

            return g;
        } else
            throw std::runtime_error("Unknown problem descriptor " + descriptor);
    }
}

Boost::Graph toBoostGraph(const Graph& g) {
    Boost::Graph ret(g.realSize());

    for (int v : g.undecided()) {
        for (int u : g.adjacent(v)) {
            if (u < v) {
                boost::add_edge(u, v, ret);
            }
        }
    }

    return ret;
}

}
