#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>

const int arraySize = 100;

void incrementRange(std::vector<int> &arr, int startIdx, int endIdx) {
    for (int i = startIdx; i < endIdx; ++i) {
        arr[i]++;
    }
}

int main() {
    std::vector<int> myArray(arraySize);

    // Initialize array
    for (int i = 0; i < arraySize; ++i) {
        myArray[i] = i;
    }

    auto start = std::chrono::steady_clock::now();
    // Perform multiple operations
    for (int operation = 0; operation < 5000000; ++operation) {
        // Submit tasks to the thread pool
        for (int t = 0; t < 5; ++t) {
            int startIdx = t * (arraySize / 5);
            int endIdx = (t + 1) * (arraySize / 5);
            incrementRange(myArray, startIdx, endIdx);
        }
    }
    auto end = std::chrono::steady_clock::now();
    double total = std::chrono::duration<double>(end - start).count();
    std::cout << "total = " << total << "\n";

    // Print modified array
    for (int i = 0; i < arraySize; ++i) {
        std::cout << myArray[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
