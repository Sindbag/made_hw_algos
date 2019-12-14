#include <iostream>
#include <vector>
#include <algorithm>

namespace {
    using std::endl;
    using std::cin;
    using std::cout;
    using std::vector;
    using std::pair;

    template<typename T, typename W>
    struct TWeightedEdge {
        T from;
        T to;
        W cost;

        bool operator<(const TWeightedEdge& other) const {
            return cost < other.cost;
        }
    };

    template<typename T, typename W>
    using TGraph = vector<TWeightedEdge<T, W>>;

    template<typename T, typename W>
    class TDSU {
    private:
        vector<T> parents;
        vector<W> ranks;

    public:
        TDSU() = delete;

        explicit TDSU(size_t size) : parents(size), ranks(size, 0) {
            for (size_t i = 0; i < size; ++i) parents[i] = i;
        }

        int Find(const T& node);
        void Unite(T a, T b);
        pair<TGraph<T, W>, W> FindMST(TGraph<T, W>& edges);
    };

    template<typename T, typename W>
    int TDSU<T, W>::Find(const T& node) {
        if (node == parents[node]) return node;
        return parents[node] = Find(parents[node]);
    }

    template<typename T, typename W>
    void TDSU<T, W>::Unite(T a, T b) {
        a = Find(a);
        b = Find(b);
        if (a != b) {
            if (ranks[a] < ranks[b]) {
                std::swap(a, b);
            }
            parents[b] = a;
            if (ranks[a] == ranks[b]) ++ranks[a];
        }
    }

    template<typename T, typename W>
    pair<TGraph<T, W>, W> TDSU<T, W>::FindMST(TGraph<T, W>& edges)  {
        std::sort(edges.begin(), edges.end());
        int sumCost = 0;
        TGraph<T, W> res;

        for (const auto& edge : edges) {
            T from = edge.from, to = edge.to;
            W cost = edge.cost;
            if (Find(from) != Find(to)) {
                sumCost += cost;
                res.push_back(edge);
                Unite(from, to);
            }
        }

        return {res, sumCost};
    }

    template<typename T, typename W>
    void readGraph(TGraph<T, W>& edges, const size_t& size) {
        for (size_t i = 0; i < size; ++i) {
            T a, b;
            W c;
            cin >> a >> b >> c;
            a--, b--;
            edges.push_back({a, b, c});
        }
    }
}

int main() {
    int n, m;
    std::cin >> n >> m;
    TDSU<int, int> dsu(n);

    TGraph<int, int> edges;
    readGraph(edges, m);

    auto res = dsu.FindMST(edges);
    std::cout << res.second << std::endl;
}