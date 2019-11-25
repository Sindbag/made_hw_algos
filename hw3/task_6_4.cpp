#include <iostream>
#include <vector>
#include <queue>

namespace {
    using std::queue;
    using std::deque;

    template<typename T>
    class TTree {
    private:
        struct TNode {
            T value;
            TNode* left = nullptr;
            TNode* right = nullptr;

            explicit TNode(const T& value_) : value(value_) {}
            TNode(TNode&) = delete;
            TNode(TNode&&) = delete;
        };
        TNode* root = nullptr;

    public:
        TTree() = default;
        ~TTree();

    private:
        void cut();

    public:
        void traversal();
        void addNode(const T& value);
    };

    template<typename T>
    TTree<T>::~TTree() {
        cut();
    }

    template<typename T>
    void TTree<T>::cut() {
        if (root == nullptr) return;

        queue toRemove(std::deque{root});
        while (!toRemove.empty()) {
            const auto& curr = toRemove.front();
            if (curr != nullptr) {
                toRemove.push(curr->left);
                toRemove.push(curr->right);
                delete curr;
            }
            toRemove.pop();
        }
    }

    template<typename T>
    void TTree<T>::traversal() {
        queue que(deque{root});
        while (!que.empty()) {
            const auto& curr = que.front();
            if (curr) {
                printf("%d ", curr->value);
                que.push(curr->left);
                que.push(curr->right);
            }
            que.pop();
        }
    }

    template<typename T>
    void TTree<T>::addNode(const T& value) {
        auto* newNode = new TTree::TNode(value);

        if (root == nullptr) {
            root = newNode;
            return;
        }

        auto curr = root;
        while (true) {
            if (curr->value >= value) {
                if (curr->left) {
                    curr = curr->left;
                } else {
                    curr->left = newNode;
                    break;
                }
            } else {
                if (curr->right) {
                    curr = curr->right;
                } else {
                    curr->right = newNode;
                    break;
                }
            }
        }
    }
}

int main() {
    TTree<int32_t> tree;
    int32_t tmp;

    std::cin >> tmp;
    while (std::cin >> tmp) {
        tree.addNode(tmp);
    }

    tree.traversal();
    return 0;
}
