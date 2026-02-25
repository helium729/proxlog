#include "minitest.h"
#include <iostream>

// Declarations of test functions from other files
void test_threadpool_execute();
void test_parse_simple_request();
void test_parse_connect_request();
void test_parse_malformed_port();
void test_large_header();
void test_proxy_integration();

int main() {
    std::cout << "=== Running All Tests ===" << std::endl;

    minitest::run_test("ThreadPool Execute Tasks", test_threadpool_execute);

    minitest::run_test("Simple Request", test_parse_simple_request);
    minitest::run_test("CONNECT Request", test_parse_connect_request);
    minitest::run_test("Malformed Port", test_parse_malformed_port);
    minitest::run_test("Large Header", test_large_header);

    minitest::run_test("Proxy Integration Test", test_proxy_integration);

    std::cout << "-----------------------------------" << std::endl;
    std::cout << "Tests Passed: " << minitest::g_passed << std::endl;
    std::cout << "Tests Failed: " << minitest::g_failed << std::endl;

    return minitest::g_failed == 0 ? 0 : 1;
}
