#include <iostream>
#include <vector>
#include <queue>

namespace {
    using std::queue;
    using std::vector;
    using std::pair;

    uint32_t countShortest(const vector<vector<int32_t>>& edges,
                           const int32_t& from, const int32_t& to) {
        vector<int32_t> visited(edges.size(), edges.size() + 1);
        visited[from] = 0;

        vector<int32_t> count(edges.size(), 0);
        count[from] = 1;

        int32_t curr;

        queue<int32_t> traverse;
        traverse.push(from);
        while (!traverse.empty()) {
            curr = traverse.front();
            traverse.pop();

            for (const auto& edge: edges[curr]) {
                if (visited[curr] + 1 < visited[edge]) {
                    visited[edge] = visited[curr] + 1;
                    count[edge] = count[curr];
                    traverse.push(edge);
                } else if (visited[curr] + 1 == visited[edge]) {
                    count[edge] = count[edge] + count[curr];
                    traverse.push(edge);
                }
            }
        }

        curr = 0;
        for (const auto& edge: edges[to]) {
            if (visited[edge] == visited[to] - 1) {
                curr += count[edge];
            }
        }

        return curr;
    }
}

int main() {
    size_t n, m;
    std::cin >> n >> m;

    std::vector<std::vector<int32_t>> edges(n);
    for (size_t i = 0; i < m; ++i) {
        int32_t a, b;
        std::cin >> a >> b;
        edges[a].push_back(b);
        edges[b].push_back(a);
    }

    int32_t from, to;
    std::cin >> from >> to;

    std::cout << countShortest(edges, from, to) << std::endl;

    return 0;
}
