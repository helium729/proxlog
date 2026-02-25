#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>

#include "thread_pool.h"
#include "minitest.h"

void test_threadpool_execute() {
    proxlog::ThreadPool pool(4);
    std::atomic<int> counter(0);

    for (int i = 0; i < 100; ++i) {
        pool.enqueue([&counter] {
            counter++;
        });
    }

    // Wait for tasks to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Check if counter reached 100
    minitest::assert_eq(counter.load(), 100, "ThreadPool counter failed");
}

// int main() {
//     minitest::run_test("ThreadPool Execute Tasks", test_threadpool_execute);

//     std::cout << "-----------------------------------" << std::endl;
//     std::cout << "Tests Passed: " << minitest::g_passed << std::endl;
//     std::cout << "Tests Failed: " << minitest::g_failed << std::endl;

//     return minitest::g_failed == 0 ? 0 : 1;
// }
