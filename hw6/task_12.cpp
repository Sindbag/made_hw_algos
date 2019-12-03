#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

namespace {
    using std::vector;
    using std::pair;

    using Graph = vector<vector<int32_t>>;
    using Edge = pair<int32_t, int32_t>;

    class TBridgeCounter {
    private:
        uint32_t timer = 0;
        const Graph& Edges;
        vector<bool> Visited;
        vector<uint32_t> Income;
        vector<uint32_t> Upcome;
        vector<Edge> Bridges;

    public:
        TBridgeCounter() = delete;
        explicit TBridgeCounter(const vector<vector<int32_t>>& edges)
            : Edges(edges)
            , Visited(edges.size(), false)
            , Income(edges.size(), edges.size() + 1)
            , Upcome(edges.size(), edges.size() + 1)
        {}

        const auto& getBridges() {
            for (int32_t i = 0; i < Edges.size(); ++i) {
                if (!Visited[i]) depthFirstSearch(i);
            }
            return Bridges;
        }

    private:
        void depthFirstSearch(int32_t from, int32_t prev = -1) {
            Visited[from] = true;
            Income[from] = Upcome[from] = timer++;
            for (const auto& to: Edges[from]) {
                if (to == prev) continue;
                if (Visited[to]) {
                    Upcome[from] = std::min(Upcome[from], Income[to]);
                    continue;
                }
                depthFirstSearch(to, from);
                Upcome[from] = std::min(Upcome[from], Upcome[to]);
                if (Upcome[to] > Income[from]) {
                    Bridges.emplace_back(from, to);
                }
            }
        }
    };
}

int main() {
    std::ifstream inFile("bridges.in");
    size_t n, m;
    inFile >> n >> m;

    Graph edges(n);
    std::map<Edge, size_t> numbs;
    std::vector<int32_t> counts(m + 1, 0);
    for (size_t i = 0; i < m; ++i) {
        int32_t a, b;
        inFile >> a >> b;
        a--, b--;
        if (numbs.find({a, b}) == numbs.end()) {
            numbs[{a, b}] = i + 1;
            numbs[{b, a}] = i + 1;
        }
        counts[numbs[{a, b}]]++;
        edges[a].push_back(b);
        edges[b].push_back(a);
    }

    std::ofstream outFile("bridges.out");
    auto counter = TBridgeCounter(edges);
    const auto& bridges = counter.getBridges();

    // remove multiple edges
    size_t count = 0;
    for (const auto& vec : bridges) {
        if (counts[numbs[vec]] == 1) {
            counts[numbs[vec]] = -1;
            count++;
        }
    }
    outFile << count << '\n';
    for (size_t i = 0; i < m + 1; ++i) {
        if (counts[i] == -1) outFile << i << std::endl;
    }

    return 0;
}
