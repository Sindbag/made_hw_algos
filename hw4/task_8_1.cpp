#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace {
    const uint64_t M = 1ull << 52ull;
    const uint64_t A = 7000111357;

    uint64_t hashFunction(const std::string& input, const uint64_t& m) {
        uint64_t hash = 0;
        auto it = input.begin();
        for (; *it != 0; ++it) {
            hash = (hash * A + *it) % m;
        }
        return hash;
    }

    template<typename THashFunctor>
    class THashTable {
    public:
        THashTable(size_t initial_size, THashFunctor hashop);
        ~THashTable();

        THashTable(const THashTable&) = delete;
        THashTable(THashTable&&) = delete;
        THashTable& operator=(const THashTable&) = delete;
        THashTable& operator=(THashTable&&) = delete;

        [[nodiscard]]
        bool Has(const std::string& key) const;
        bool Add(const std::string& key);
        bool Remove(const std::string& key);

    private:
        struct THashTableNode {
            std::string key;
            uint64_t hash;
            bool deleted;

            THashTableNode(std::string key_, const uint64_t& hash_)
                : key(std::move(key_))
                , hash(hash_)
                , deleted(false)
            {}
        };

        std::vector<THashTableNode*> table;
        THashFunctor op;
        uint64_t keyCount = 0;
        float th = 0.75;

        [[nodiscard]]
        inline uint64_t probe(const size_t& i, const uint64_t& prev, const size_t& size) const {
            return (prev + i) % size;
        }

        [[nodiscard]]
        std::pair<bool, uint64_t> searchPresent(const std::string& key,
                                                const uint64_t& hash,
                                                const std::vector<THashTableNode*>& tab) const {
            auto pos = hash % tab.size();
            for (size_t i = 0; tab[pos] != nullptr; pos = probe(++i, pos, tab.size())) {
                if (tab[pos]->key == key) {
                    if (tab[pos]->deleted) {
                        return {false, pos}; // found, but deleted, return empty slot
                    }
                    return {true, pos}; // found, return position
                }
            }
            return {false, pos}; // not found, return empty position
        }
    };

    template<typename THashFunctor>
    THashTable<THashFunctor>::THashTable(size_t initial_size, THashFunctor hashop)
        : table(initial_size, nullptr)
        , op(hashop)
    {}

    template<typename THashFunctor>
    THashTable<THashFunctor>::~THashTable() {
        for (THashTableNode* node : table) {
            delete node;
        }
    }

    template<typename THashFunctor>
    bool THashTable<THashFunctor>::Has(const std::string& key) const {
        assert(!key.empty());

        const auto hash = op(key, M);
        auto res = searchPresent(key, hash, table);
        return res.first;
    }

    template<typename THashFunctor>
    bool THashTable<THashFunctor>::Add(const std::string& key) {
        assert(!key.empty());

        const auto hash = op(key, M);
        auto res = searchPresent(key, hash, table);
        if (res.first) { // already in table
            return false;
        }

        if (table[res.second]) {
            table[res.second]->key = key;
            table[res.second]->hash = hash;
            table[res.second]->deleted = false;
        } else {
            auto* newNode = new THashTableNode(key, hash);
            table[res.second] = newNode;
            keyCount += 1;
        }

        // time to grow
        if (keyCount * (1.0 / table.size()) >= th) {
            std::vector<THashTableNode*> newTab(table.size() * 2);
            for (size_t i = 0; i < table.size(); ++i) {
                if (table[i] && !table[i]->deleted) {
                    newTab[searchPresent(table[i]->key, table[i]->hash, newTab).second] = table[i];
                }
            }
            table = newTab;
        }

        return true;
    }

    template<typename THashFunctor>
    bool THashTable<THashFunctor>::Remove(const std::string& key) {
        assert(!key.empty());

        const auto hash = op(key, M);
        auto res = searchPresent(key, hash, table);
        if (!res.first) {
            return false;
        }

        table[res.second]->deleted = true;
        return true;
    }
}

int main() {
    THashTable table(8, hashFunction);
    char command = ' ';
    std::string value;
    while (std::cin >> command >> value) {
        switch (command) {
            case '?':
                std::cout << (table.Has(value) ? "OK" : "FAIL") << std::endl;
                break;
            case '+':
                std::cout << (table.Add(value) ? "OK" : "FAIL") << std::endl;
                break;
            case '-':
                std::cout << (table.Remove(value) ? "OK" : "FAIL") << std::endl;
                break;
            default:
                break;
        }
    }
    return 0;
}