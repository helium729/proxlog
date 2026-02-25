#include "minitest.h"
#include <iostream>

namespace minitest {

int g_passed = 0;
int g_failed = 0;

void run_test(const std::string& name, std::function<void()> test) {
    std::cout << "[RUNNING] " << name << "...";
    try {
        test();
        std::cout << " OK" << std::endl;
        g_passed++;
    } catch (const std::exception& e) {
        std::cout << " FAILED: " << e.what() << std::endl;
        g_failed++;
    } catch (...) {
        std::cout << " FAILED: Unknown exception" << std::endl;
        g_failed++;
    }
}

void assert_true(bool condition, const std::string& msg) {
    if (!condition) throw std::runtime_error(msg);
}

void assert_eq(const std::string& a, const std::string& b, const std::string& msg) {
    if (a != b) throw std::runtime_error(msg + " (Expected: " + a + ", Got: " + b + ")");
}

void assert_eq(int a, int b, const std::string& msg) {
    if (a != b) throw std::runtime_error(msg + " (Expected: " + std::to_string(a) + ", Got: " + std::to_string(b) + ")");
}

} // namespace minitest
