#include <vector>
#include <iostream>

namespace {
    using namespace std;

    template<typename T>
    void swap(vector<T>& arr, int i, int j) {
        T t = arr[i];
        arr[i] = arr[j];
        arr[j] = t;
    }

    template<typename T>
    int getMid(const T& a, const T& b, const T& c) {
        if (a > b) {
            if (b > c) {
                return 1;
            } else {
                if (a > c) {
                    return 2;
                }
                return 0;
            }
        } else {
            if (b < c) {
                return 1;
            } else {
                if (a < c) {
                    return 2;
                }
                return 0;
            }
        }
    }

    template<typename T>
    int getPivot(vector<T>& arr, int l, int h) {
        return vector<int>{l, h, l + (h-l)/2}[getMid(arr[l], arr[h], arr[l + (h - l) / 2])];
    }

    template<typename T>
    int partition(vector<T>& arr, int l, int h) {
        int x = getPivot(arr, l, h); // idx

        // swap pivot with last element
        swap(arr, x, h);
        int i = l;
        int j;

        //find first higher
        while (i < h && arr[i] <= arr[h]) { i++; }
        j = i;

        while (j < h) {
            if (arr[j] > arr[h]) {
                j++;
            } else {
                swap(arr, i, j);
                i++;
                j++;
            }
        }
        swap(arr, i, h);
        return i;
    }

    template<typename T>
    int getKStat(vector<T>& arr, int left, int right, int k) {
        int p;
        do {
            p = partition(arr, left, right);
            if (p > k) {
                right = p - 1;
            }
            if (p < k) {
                left = p + 1;
            }
        } while (p != k);
        return arr[k];
    }
}

int main() {
    int n, k;
    std::cin >> n >> k;
    vector<int> arr(n);
    for (auto& el : arr) { std::cin >> el; }
    cout << getKStat(arr, 0, n - 1, k);
    return 0;
}