#include <iostream>
#include <vector>
#include <queue>

namespace {
    using std::queue;

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

            void print() const {
                printf("%d ", value);
            }
        };
        TNode* root = nullptr;

    public:
        TTree() = default;
        ~TTree();

    private:
        template<typename Callback>
        void traversalCb(Callback cb);
        void cut();

    public:
        void print();
        void addNode(const T& value);
    };

    template<typename T>
    TTree<T>::~TTree() {
        cut();
    }

    template<typename T>
    template<typename Callback>
    void TTree<T>::traversalCb(Callback cb) {
        queue<TNode*> toProcess;
        toProcess.push(root);
        while (!toProcess.empty()) {
            const auto& curr = toProcess.front();
            if (curr != nullptr) {
                toProcess.push(curr->left);
                toProcess.push(curr->right);
                cb(curr);
            }
            toProcess.pop();
        }

    }

    template<typename T>
    void TTree<T>::cut() {
        traversalCb([&](TTree::TNode* el) { delete el; });
    }

    template<typename T>
    void TTree<T>::print() {
        traversalCb([&](TTree::TNode* el) { el->print(); });
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
    int32_t n, tmp;

    std::cin >> n;
    while (n-- && std::cin >> tmp) {
        tree.addNode(tmp);
    }

    tree.print();
    return 0;
}
