#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <cmath>

constexpr uint64_t HashParameter = 11;

struct position {
    std::vector<char> chips;
    char zero_place;

    bool IsFinish() const;
    std::vector<position> Siblings() const;

    bool operator==(const position& other) const;
    bool operator!=(const position& other) const { return !operator==(other); }
};

struct weighted {
    int weight;
    position pos;

    bool operator<(const weighted& other) const {
        return weight < other.weight;
    }
};

constexpr uint64_t HashParams[] = {
        1,
        HashParameter,
        HashParameter * HashParameter,
        HashParameter * HashParameter * HashParameter,
        HashParameter * HashParameter * HashParameter * HashParameter,
        HashParameter * HashParameter * HashParameter * HashParameter * HashParameter,
        HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter,
        HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter,
        HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter,
        HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter,
        HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter,
        HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter,
        HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter,
        HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter,
        HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter,
        HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter,
        HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter * HashParameter
};

namespace std {
    template <>
    struct hash<position> {
        size_t operator()(const position& key) const {
            size_t value = 0;
            for (size_t i = 0; i < key.chips.size(); ++i) {
                value += HashParams[i] * static_cast<size_t>(key.chips[i]);
            }
            return value;
        }
    };
}

const position FinishPosition{{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0}, 15};

char GetMoveSymbol(const position& from, const position& to);

bool position::IsFinish() const {
    return operator==(FinishPosition);
}

std::vector<position> position::Siblings() const {
    std::vector<position> result;
    if (zero_place < 12) {
        position sibling = *this;
        std::swap(sibling.chips[zero_place], sibling.chips[zero_place + 4]);
        sibling.zero_place += 4;
        result.emplace_back(sibling);
    }
    if (zero_place >= 4) {
        position sibling = *this;
        std::swap(sibling.chips[zero_place], sibling.chips[zero_place - 4]);
        sibling.zero_place -= 4;
        result.emplace_back(sibling);
    }
    if (zero_place % 4 != 0) {
        position sibling = *this;
        std::swap(sibling.chips[zero_place], sibling.chips[zero_place - 1]);
        sibling.zero_place -= 1;
        result.emplace_back(sibling);
    }
    if (zero_place % 4 != 3) {
        position sibling = *this;
        std::swap(sibling.chips[zero_place], sibling.chips[zero_place + 1]);
        sibling.zero_place += 1;
        result.emplace_back(sibling);
    }
    return result;
}

bool position::operator==(const position& other) const {
    for (size_t i = 0; i < chips.size(); ++i ) {
        if (chips[i] != other.chips[i]) return false;
    }
    return true;
}

char GetMoveSymbol(const position& from, const position& to) {
    char zero_diff = to.zero_place - from.zero_place;
    switch (zero_diff) {
        case 1:
            return 'R'; // Ноль вправо -> фишка влево
        case -1:
            return 'L';
        case 4:
            return 'D';
        case -4:
            return 'U';
        default:
            assert(false);
    }
    return 0;
}


int linear(const std::vector<char> b, const char& n = 4) {
    int count = 0;
    std::vector<char> inCol(n * n), inRow(n * n);

    for (char y = 0; y < n; ++y) {
        for (char x = 0; x < n; ++x) {
            char i = y * n + x;

            if (b[i] == 0) {
                continue;
            }

            char bX = 0, bY = 0;
            if (b[i] % n == 0) {
                bX = n - 1;
                bY = b[i] / n - 1;
            } else {
                bX = b[i] % n - 1;
                bY = b[i] / n;
            }

            inCol[i] = (bX == x);
            inRow[i] = (bY == y);
        }
    }

    for (char y = 0; y < n; ++y) {
        for (char x = 0; x < n; ++x) {
            char i = y * n + x;

            if (b[i] == 0) {
                continue;
            }

            if (inCol[i]) {
                for (auto z = y; z < n; ++z) {
                    auto j = z * n + x;
                    if (b[j] == 0) {
                        continue;
                    }

                    if (inCol[j]) {
                        if ((b[j] < b[i]) && ((abs(b[j] - b[i]) % n) == 0)) {
                            ++count;
                        }
                    }
                }
            }

            if (inRow[i]) {
                char bI = b[i];

                for (char z = x + 1; z < n; ++z) {
                    char j = y * n + z;
                    char bJ = b[j];

                    if (b[j] == 0) {
                        continue;
                    }

                    if (inRow[j]) {
                        if ((bJ < bI) && (0 <= (bI - bJ)) && ((bI - bJ) < n)) {
                            ++count;
                        }
                    }
                }
            }
        }
    }

    return 2 * count;
}


int heuristic(const position& sibling) {
    int sum = 0;
    for (char i = 0; i < sibling.chips.size(); ++i) {
        sum += abs(sibling.chips[i] % 4 - i % 4) + abs(sibling.chips[i] / 4  - i / 4);
    }

    sum += linear(sibling.chips);

    return sum;
}

bool noSolution(const position& a) {
    int inv = 0;
    for (int i = 0; i < 16; ++i)
        if (a.chips[i])
            for (int j = 0; j < i; ++j)
                if (a.chips[j] > a.chips[i])
                    ++inv;
    for (int i = 0; i < 16; ++i)
        if (a.chips[i] == 0)
            inv += 1 + i / 4;

    return inv % 2;
}

bool BFS(const position& start, std::unordered_map<position, position>& parents) {
    std::priority_queue<weighted, std::vector<weighted>> positions_queue;
    positions_queue.push({80, start});
    while (!positions_queue.empty()) {
        weighted current = positions_queue.top();
        positions_queue.pop();
        for (const auto& sibling : current.pos.Siblings()) {
            if (parents.find(sibling) != parents.end()) {
                continue;
            }
            parents[sibling] = current.pos;
            if (sibling.IsFinish()) {
                return true;
            }
            auto heur = heuristic(sibling);
            if (heur <= current.weight + 10) positions_queue.push({heur, sibling});
        }
    }
    return false;
}

std::vector<char> GetPath(const position& start,
                          const std::unordered_map<position, position>& parents) {
    std::vector<char> result;

    position current = FinishPosition;
    while (current != start) {
        const position& parent = parents.at(current);
        result.push_back(GetMoveSymbol(parent, current));
        current = parent;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

std::pair<bool, std::vector<char>> SolvePuzzle15(const position& start) {
    std::unordered_map<position, position> parents;
    if (!BFS(start, parents)) {
        return std::make_pair(false, std::vector<char>());
    }
    return std::make_pair(true, GetPath(start, parents));
}

int main() {
    char size = 16, nullpos = -1;
    std::vector<char> st(size);

    int tmp;
    for (char i = 0; i < size; ++i) {
        std::cin >> tmp;
        st[i] = tmp;
        if (st[i] == 0) {
            nullpos = i;
        }
    }

    position start{st, static_cast<char>(nullpos)};
    if (noSolution(start)) {
        std::cout << "-1" << std::endl;
        return 0;
    }

    const auto result = SolvePuzzle15(start);
    std::cout << result.second.size() << std::endl;
    for (const auto move : result.second) {
        std::cout << move;
    }
    return 0;
}

// 1 2 3 0 5 6 7 4 9 10 11 8 13 14 15 12
// 1 2 3 0 5 6 7 4 9 10 11 8 13 14 12 15
// 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 0
// 15 2 1 12 8 5 6 11 4 9 10 7 3 13 14 0