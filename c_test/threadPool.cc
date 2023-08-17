#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    ThreadPool(size_t numThreads) : stop(false) {
        for (size_t i = 0; i < numThreads; ++i) {
            threads.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) {
                            return;
                        }
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker : threads) {
            worker.join();
        }
    }

    template <class F>
    void enqueue(F&& f) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.emplace(std::forward<F>(f));
        }
        condition.notify_one();
    }

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};

const int arraySize = 100;
const int numThreads = 5;
const int elementsPerThread = arraySize / numThreads;
std::vector<int> myArray(arraySize);

void incrementRange(int startIdx, int endIdx) {
    for (int i = startIdx; i < endIdx; ++i) {
        myArray[i]++;
    }
}

int main() {
    // Initialize array
    for (int i = 0; i < arraySize; ++i) {
        myArray[i] = i;
    }

    ThreadPool threadPool(numThreads);

    auto start = std::chrono::steady_clock::now();
    // Perform multiple operations
    for (int operation = 0; operation < 5000000; ++operation) {
        // Submit tasks to the thread pool
        for (int t = 0; t < numThreads; ++t) {
            int startIdx = t * elementsPerThread;
            int endIdx = (t + 1) * elementsPerThread;
            threadPool.enqueue([startIdx, endIdx] {
                incrementRange(startIdx, endIdx);
            });
        }
    }
    auto end = std::chrono::steady_clock::now();
    double total = std::chrono::duration<double>(end - start).count();
    std::cout << "total = " << total << "\n";
    // Wait for all tasks to finish

    // Print modified array
    for (int i = 0; i < arraySize; ++i) {
        std::cout << myArray[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
