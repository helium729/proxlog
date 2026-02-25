// Remove main() from individual test files to allow linking into a single runner

void test_threadpool_execute(); // Defined in test_threadpool.cc
void test_parse_simple_request(); // Defined in test_http_request.cc
void test_parse_connect_request(); // Defined in test_http_request.cc
void test_parse_malformed_port(); // Defined in test_http_request.cc
void test_large_header(); // Defined in test_http_request.cc
void test_proxy_integration(); // Defined in test_integration.cc
