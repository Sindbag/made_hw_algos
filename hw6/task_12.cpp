#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>

namespace {
    using std::vector;
    using std::pair;

    using Graph = vector<vector<int32_t>>;
    using Edge = pair<int32_t, int32_t>;


    class TBridgeCounter {
    private:
        struct Step {
            int from = 0;
            int to = 0;
            int depth = 0;
        };

        uint32_t Timer = 0;
        const Graph& Edges;
        vector<bool> Visited;
        vector<uint32_t> DiscoveryTime;
        vector<uint32_t> MinimumTime;
        vector<Edge> Bridges;

    public:
        TBridgeCounter() = delete;
        explicit TBridgeCounter(const vector<vector<int32_t>>& edges)
            : Edges(edges)
            , Visited(edges.size(), false)
            , DiscoveryTime(edges.size(), edges.size() + 1)
            , MinimumTime(edges.size(), edges.size() + 1)
        {}

        const auto& getBridges() {
            for (size_t i = 0; i < Edges.size(); ++i) {
                if (!Visited[i]) depthFirstSearch(i);
            }
            return Bridges;
        }

        void checkBridge(int p, int v) {
            if (p < 0 || v < 0) return;
            MinimumTime[v] = std::min(MinimumTime[v], MinimumTime[p]);
            if (MinimumTime[p] > DiscoveryTime[v]) {
                Bridges.emplace_back(p, v);
            }
        }

    private:
        void depthFirstSearch(int32_t from, int32_t prev = -1) {
            std::stack<Step> order;
            order.push(Step{prev, from, 0});

            int prevDepth = -1;
            do {
                Step tmp = order.top();
                order.pop();

                bool check = tmp.depth < prevDepth;
                prevDepth = tmp.depth;

                if (check) {
                    // step back from dfs
                    checkBridge(tmp.to, tmp.from);
                    continue;
                }

                // step further
                if (Visited[tmp.to]) {
                    MinimumTime[tmp.from] = std::min(MinimumTime[tmp.from], DiscoveryTime[tmp.to]);
                    continue;
                }

                Visited[tmp.to] = true;
                DiscoveryTime[tmp.to] = MinimumTime[tmp.to] = Timer++;

                if (Edges[tmp.to].size() != 1 || tmp.from != Edges[tmp.to][0]) order.push({tmp.from, tmp.to, tmp.depth});
                else checkBridge(tmp.to, tmp.from);

                for (size_t i = 0; i < Edges[tmp.to].size(); ++i)
                    if (tmp.from != Edges[tmp.to][i])
                        order.push({tmp.to, Edges[tmp.to][i], tmp.depth + 1});
            } while (!order.empty());
        }
    };
}

int main() {
    std::string PREFIX = "/Users/sind/CLionProjects/extsearch/hw/hw6/";
    std::ifstream inFile(PREFIX + "bridges.in");
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

    std::ofstream outFile(PREFIX + "bridges.out");
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
