#include <iostream>
#include <vector>
#include <cmath>

namespace {
    template<typename T>
    inline void printVec(const std::vector<T>& vec) {
        for (const auto& el : vec) {
            std::cout << el << ' ';
        }
        std::cout << std::endl;
    }

    template<typename T>
    void merge(std::vector<T>& arr, const int begin, const int mid, const int end) {
        const std::vector<T> left(arr.begin() + begin, arr.begin() + mid);
        const std::vector<T> right(arr.begin() + mid, arr.begin() + end);

        auto leftIter = left.begin();
        auto rightIter = right.begin();
        int target = begin;

        while (leftIter != left.end() && rightIter != right.end()) {
            if (*leftIter <= *rightIter) {
                arr[target++] = *(leftIter++);
            }
            else {
                arr[target++] = *(rightIter++);
            }
        }

        while (leftIter != left.end()) arr[target++] = *(leftIter++);
        while (rightIter != right.end()) arr[target++] = *(rightIter++);
    }

    template<typename T>
    void mergeSort(std::vector<T>& arr, const int from, const int to) {
        if (to - from > 1) {
            int half = from + (to - 1 - from) / 2 + 1;
            mergeSort(arr, from, half);
            mergeSort(arr, half, to);
            merge(arr, from, half, to);
        }
    }
}

int main() {
    int n, k;
    std::cin >> n >> k;
    std::vector<int> arr(n);
    for (auto& el : arr) {
        std::cin >> el;
    }

    for (int i = 0; i < n; i += k) {
        mergeSort(arr, i, std::min(n, i + k));
    }

    for (int i = 0; i + k < n; i += k) {
        merge(arr, i, i + k, std::min(n, i + 2 * k));
    }

    printVec(arr);
    return 0;
}