#include <iostream>
#include <vector>
#include <queue>

namespace {
    template<typename T>
    class TNode {
    public:
        T Value;
        TNode<T>* left = nullptr;
        TNode<T>* right = nullptr;

        TNode() = default;
        explicit TNode(T val)
            : Value(val)
        {}
    };

    template<typename T>
    void traversal(const TNode<T>* root) {
        std::queue<const TNode<T>*> que;
        que.push(root);
        while (!que.empty()) {
            auto curr = que.front();
            que.pop();
            std::cout << curr->Value << ' ';
            if (curr->left) { que.push(curr->left); }
            if (curr->right) { que.push(curr->right); }
        }
    }

    template<typename T>
    void addNode(TNode<T>* root, T value) {
        auto curr = root;
        auto* newNode = new TNode(value);
        while (true) {
            if (curr->Value >= value) {
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
    int n, tmp;
    std::cin >> n;
    std::cin >> tmp;
    int i = 1;
    auto* root = new TNode(tmp);
    while (i < n) {
        std::cin >> tmp;
        addNode(root, tmp);
        i++;
    }
    traversal(root);
    return 0;
}