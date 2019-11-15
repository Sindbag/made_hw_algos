#include <iostream>

namespace {
    using namespace std;

    class TNode {
    public:
        int key;
        int count;
        TNode* left = nullptr;
        TNode* right = nullptr;

        explicit TNode(const int k_)
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
        int getCount() {
            count = 1 + (left != nullptr ? left->count : 0) + (right != nullptr ? right->count : 0);
            return count;
        }
    };

    TNode* rotateRight(TNode* x) {
        TNode* y = x->left;
        x->setLeft(y->right);
        y->setRight(x);
        return y;
    }

    TNode* rotateLeft(TNode* x) {
        TNode* y = x->right;
        x->setRight(y->left);
        y->setLeft(x);
        return y;
    }

    TNode* splay(TNode* root, int key) {
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

    TNode* insert(TNode* root, int k) {
        if (root == nullptr) {
            return new TNode(k);
        }

        root = splay(root, k);
        if (root->key == k) return root;

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
        return newNode;
    }

    TNode* deleteKey(TNode* root, int key) {
        if (root == nullptr)
            return nullptr;

        root = splay(root, key);
        if (key != root->key)
            return root;

        TNode* temp;
        if (root->left == nullptr) {
            temp = root;
            root = root->right;
        } else {
            temp = root;
            root = splay(root->left, key);
            root->setRight(temp->right);
        }
        free(temp);
        return root;
    }

    TNode* findStat(TNode* root, int k) {
        if (root == nullptr) {
            return root;
        }

        int left = 0;
        if (root->left != nullptr) {
            left = root->left->count;
        }
        if (root->count < k) {
            return nullptr;
        }

        if (k == left) {
            return root;
        }
        return left < k
               ? findStat(root->right, k - left - 1)
               : findStat(root->left, k);
    }
}

int main() {
    TNode *root = nullptr;
    int n;
    std::cin >> n;
    int a, b;
    for (size_t i = 0; i < n; ++i) {
        std::cin >> a >> b;
        if (a < 0) {
            root = deleteKey(root, abs(a));
        } else {
            root = insert(root, a);
        }
        auto* stat = findStat(root, b);
        std::cout << stat->key << endl;
    }
    return 0;
}
