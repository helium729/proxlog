# cmake version >= 3.5
cmake_minimum_required(VERSION 3.5)

# project name
project(proxlog)

# set source file directory
aux_source_directory(src DIR_SRCS)

# set include directory
include_directories(include)
include_directories(tests)
link_directories(lib)

# find gtest library
# find_package(GTest REQUIRED)
# include_directories(${GTEST_INCLUDE_DIRS})
# link_directories(${GTEST_LIBRARY_DIRS})

# add executable file
add_executable(proxlog ${DIR_SRCS} main.cc)

# add test executable file
# add_executable(test_proxlog ${DIR_SRCS} test.cc)
# target_link_libraries(test_proxlog ${GTEST_LIBRARIES} pthread)

# New Test Suite
set(TEST_SRCS
    tests/test_main.cc
    tests/minitest.cc
    tests/test_threadpool.cc
    tests/test_http_request.cc
    tests/test_integration.cc
)

add_executable(test_suite ${DIR_SRCS} ${TEST_SRCS})
target_link_libraries(test_suite pthread)
