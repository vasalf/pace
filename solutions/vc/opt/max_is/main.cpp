#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::vector<int>> graph;

std::vector<int> curAns;
std::vector<int> left;
bool findIS(int level) {
    if (level == 0)
        return true;
    if (left.empty())
        return false;
    std::vector<int> oldLeft = left;
    for (int u : oldLeft) {
        std::vector<int> newLeft;
        std::set_difference(left.begin(), left.end(), graph[u].begin(), graph[u].end(), std::back_inserter(newLeft));
        newLeft.erase(std::find(newLeft.begin(), newLeft.end(), u));
        left = newLeft;
        curAns.push_back(u);
        if (findIS(level - 1)) {
            return true;
        }
        left = oldLeft;
        curAns.pop_back();
    }
    return false;
}

int main(int argc, char* argv[]) {
    assert(argc > 1);
    int k = std::atoi(argv[1]);

    int n, m;
    while (true) {
        std::string s;
        std::getline(std::cin, s);
        if (s[0] == 'p') {
            std::istringstream ss(s);
            std::string p, td;
            ss >> p >> td >> n >> m;
            break;
        }
    }

    graph.resize(n);

    std::vector<int> aprior;
    std::vector<std::pair<int, int>> edges;
    std::string s;
    int es = 0;
    while (es < m && std::getline(std::cin, s)) {
        if (s[0] == 'c') {
            continue;
        }
        std::istringstream ss(s);
        int u, v;
        ss >> u >> v;
        u--; v--;
        if (u == v) {
            aprior.push_back(u);
        } else {
            if (u > v) {
                std::swap(u, v);
            }
            edges.push_back({u, v});
        }
        es++;
    }
    std::sort(aprior.begin(), aprior.end());
    aprior.resize(std::unique(aprior.begin(), aprior.end()) - aprior.begin());
    std::sort(edges.begin(), edges.end());
    edges.resize(std::unique(edges.begin(), edges.end()) - edges.begin());

    for (auto p : edges) {
        graph[p.first].push_back(p.second);
        graph[p.second].push_back(p.first);
    }
    std::vector<int> all(n);
    std::iota(all.begin(), all.end(), 0);
    std::set_difference(all.begin(), all.end(), aprior.begin(), aprior.end(), std::back_inserter(left));

    for (int i = 0; i < n; i++) {
        std::sort(graph[i].begin(), graph[i].end());
    }

    if (findIS(k)) {
        std::cout << "OK: ";
        for (int u : curAns) {
            std::cout << u + 1 << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "NO" << std::endl;
    }
}
