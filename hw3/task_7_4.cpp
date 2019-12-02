#include <iostream>

namespace {
    using namespace std;

    class TTree {
    private:
        class TNode {
        public:
            uint64_t key;
            uint32_t count;
            TNode* left = nullptr;
            TNode* right = nullptr;

            explicit TNode(const uint64_t k_)
                    : key(k_), count(1) {}

            void setLeft(TNode* left_) {
                left = left_;
                if (left != nullptr) {
                    left->getCount();
                }
                getCount();
            }

            void setRight(TNode* right_) {
                right = right_;
                if (right != nullptr) {
                    right->getCount();
                }
                getCount();
            }

        private:
            uint32_t getCount() {
                count = 1 + (left != nullptr ? left->count : 0) + (right != nullptr ? right->count : 0);
                return count;
            }
        };

        TNode* root = nullptr;

    public:
        void pop(const uint64_t& key) {
            if (root == nullptr) {
                return;
            }

            root = splay(root, key);
            if (key != root->key) return;

            TNode* temp;
            if (root->left == nullptr) {
                temp = root;
                root = root->right;
            } else {
                temp = root;
                root = splay(root->left, key);
                root->setRight(temp->right);
            }
            
            delete temp;
        }

        [[nodiscard]]
        const TNode* findStat(int32_t k) const {
            if (root == nullptr) {
                return root;
            }

            TNode* tmp = root;
            uint32_t left = tmp->left != nullptr ? tmp->left->count : 0;
            while (k != left && tmp->count >= k) {
                if (left < k) {
                    tmp = tmp->right;
                    k = k - left - 1;
                } else {
                    tmp = tmp->left;
                }
                if (tmp->left != nullptr) {
                    left = tmp->left->count;
                } else {
                    left = 0;
                }
            };
            return tmp;
        };

        void insert(const uint64_t& k) {
            if (root == nullptr) {
                root = new TNode(k);
                return;
            }

            root = splay(root, k);
            if (root->key == k) return;

            auto* newNode = new TNode(k);
            if (root->key > k) {
                newNode->setRight(root);
                newNode->setLeft(root->left);
                root->setLeft(nullptr);
            } else {
                newNode->setLeft(root);
                newNode->setRight(root->right);
                root->setRight(nullptr);
            }
            root = newNode;
        }

        void cutDown() {
            cutDown(root);
        }

    private:
        static void cutDown(TNode* root) {
            if (root->left != nullptr) {
                cutDown(root->left);
            }
            if (root->right != nullptr) {
                cutDown(root->right);
            }
            delete root;
        }

        static TNode* rotateRight(TNode* x) {
            TNode* y = x->left;
            x->setLeft(y->right);
            y->setRight(x);
            return y;
        }

        static TNode* rotateLeft(TNode* x) {
            TNode* y = x->right;
            x->setRight(y->left);
            y->setLeft(x);
            return y;
        }

        static TNode* splay(TNode* root, const uint64_t& key) {
            if (root == nullptr || root->key == key)
                return root;

            if (root->key > key) {
                if (root->left == nullptr) return root;
                if (root->left->key > key) {
                    // zig-zig
                    root->left->setLeft(splay(root->left->left, key));
                    root = rotateRight(root);
                } else if (root->left->key < key) {
                    // zig-zag
                    root->left->setRight(splay(root->left->right, key));
                    if (root->left->right != nullptr) {
                        root->setLeft(rotateLeft(root->left));
                    }
                }
                return (root->left == nullptr) ? root : rotateRight(root);
            } else {
                if (root->right == nullptr) return root;
                if (root->right->key > key) {
                    // zag-zig
                    root->right->setLeft(splay(root->right->left, key));
                    if (root->right->left != nullptr) {
                        root->setRight(rotateRight(root->right));
                    }
                } else if (root->right->key < key) {
                    // zag-zag
                    root->right->setRight(splay(root->right->right, key));
                    root = rotateLeft(root);
                }
                return (root->right == nullptr) ? root : rotateLeft(root);
            }
        }
    };
}

int main() {
    TTree tree;
    int n;
    std::cin >> n;
    int64_t a, b;
    for (size_t i = 0; i < n; ++i) {
        std::cin >> a >> b;
        if (a < 0) {
            tree.pop(abs(a));
        } else {
            tree.insert(a);
        }
        auto* stat = tree.findStat(b);
        std::cout << stat->key << endl;
    }
    tree.cutDown();
    return 0;
}
