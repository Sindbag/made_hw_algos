#include <iostream>
#include <vector>
#include <cmath>

namespace {
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
    int n, k, red = 0;
    std::cin >> n >> k;
    std::vector<int> arr(2 * k);
    for (size_t i = 0; i < k && red < n; ++i) {
        std::cin >> arr[k + i];
        red++;
    }
    mergeSort(arr, k, red % k ? k + red % k : 2 * k);

    for (; red < n;) {
        const auto to_read = std::min(k, n - red);
        const auto leftPad = k - to_read;
        for (size_t i = 0; i < to_read; ++i) {
            std::cin >> arr[leftPad + i];
            red++;
        }
        mergeSort(arr, leftPad, k);
        merge(arr, leftPad, k, 2 * k);
        for (size_t j = 0; j < to_read; ++j) std::cout << arr[leftPad + j] << ' ';
    }

    for (size_t j = 0; j < k; ++j) std::cout << arr[k + j] << ' ';

    return 0;
}
