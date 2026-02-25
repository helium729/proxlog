#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdio>

#include "http_message.h"
#include "minitest.h"

void test_parse_simple_request() {
    int sockets[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);

    std::string req = "GET /index.html HTTP/1.1\r\nHost: example.com\r\n\r\n";
    send(sockets[1], req.c_str(), req.length(), 0);
    close(sockets[1]);

    proxlog::HttpRequest parser(sockets[0]);
    minitest::assert_true(parser.isValid(), "Invalid parser state");
    minitest::assert_eq(parser.getMethod(), "GET", "Method mismatch");
    minitest::assert_eq(parser.getUri(), "/index.html", "URI mismatch");
    minitest::assert_eq(parser.getVersion(), "HTTP/1.1", "Version mismatch");
    minitest::assert_eq(parser.getHost(), "example.com", "Host mismatch");
    minitest::assert_eq(parser.getPort(), 80, "Port mismatch");

    close(sockets[0]);
}

void test_parse_connect_request() {
    int sockets[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);

    std::string req = "CONNECT example.com:443 HTTP/1.1\r\nHost: example.com:443\r\n\r\n";
    send(sockets[1], req.c_str(), req.length(), 0);
    close(sockets[1]);

    proxlog::HttpRequest parser(sockets[0]);
    minitest::assert_true(parser.isValid(), "Invalid parser state");
    minitest::assert_eq(parser.getMethod(), "CONNECT", "Method mismatch");
    minitest::assert_eq(parser.getUri(), "example.com:443", "URI mismatch");
    minitest::assert_eq(parser.getHost(), "example.com", "Host mismatch");
    minitest::assert_eq(parser.getPort(), 443, "Port mismatch");

    close(sockets[0]);
}

void test_parse_malformed_port() {
    int sockets[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);

    std::string req = "GET / HTTP/1.1\r\nHost: example.com:abc\r\n\r\n";
    send(sockets[1], req.c_str(), req.length(), 0);
    close(sockets[1]);

    proxlog::HttpRequest parser(sockets[0]);
    minitest::assert_true(parser.isValid(), "Invalid parser state");
    // Should default to 80 on malformed port
    minitest::assert_eq(parser.getPort(), 80, "Port mismatch on malformed input");

    close(sockets[0]);
}

void test_large_header() {
    int sockets[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);

    std::string header_val(1000, 'A');
    std::string req = "GET / HTTP/1.1\r\nX-Large-Header: " + header_val + "\r\nHost: example.com\r\n\r\n";
    send(sockets[1], req.c_str(), req.length(), 0);
    close(sockets[1]);

    proxlog::HttpRequest parser(sockets[0]);
    minitest::assert_true(parser.isValid(), "Invalid parser state");
    minitest::assert_eq(parser.getHeader("X-Large-Header"), header_val, "Large header mismatch");

    close(sockets[0]);
}

// int main() {
//     minitest::run_test("Simple Request", test_parse_simple_request);
//     minitest::run_test("CONNECT Request", test_parse_connect_request);
//     minitest::run_test("Malformed Port", test_parse_malformed_port);
//     minitest::run_test("Large Header", test_large_header);

//     std::cout << "-----------------------------------" << std::endl;
//     std::cout << "Tests Passed: " << minitest::g_passed << std::endl;
//     std::cout << "Tests Failed: " << minitest::g_failed << std::endl;

//     return minitest::g_failed == 0 ? 0 : 1;
// }
