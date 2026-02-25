#ifndef MINITEST_H
#define MINITEST_H

#include <string>
#include <functional>

namespace minitest {

extern int g_passed;
extern int g_failed;

void run_test(const std::string& name, std::function<void()> test);
void assert_true(bool condition, const std::string& msg);
void assert_eq(const std::string& a, const std::string& b, const std::string& msg);
void assert_eq(int a, int b, const std::string& msg);

} // namespace minitest

#endif
