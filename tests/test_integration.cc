#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <atomic>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include "server.h"
#include "minitest.h"

// Simple mock server to act as upstream
void mock_server_thread(int port, std::atomic<bool>& running) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Mock server bind failed");
        return;
    }
    if (listen(server_fd, 5) < 0) {
        perror("Mock server listen failed");
        return;
    }

    while (running) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) continue;

        char buffer[1024] = {0};
        read(client_fd, buffer, 1024);

        const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
        send(client_fd, response, strlen(response), 0);
        close(client_fd);
    }
    close(server_fd);
}

void test_proxy_integration() {
    int proxy_port = 9090;
    int mock_port = 9091;

    // 1. Start Mock Upstream Server
    std::atomic<bool> mock_running(true);
    std::thread mock_server(mock_server_thread, mock_port, std::ref(mock_running));

    // 2. Start Proxy Server
    std::thread proxy_server([proxy_port]() {
        proxlog::Server server(proxy_port);
        server.run();
    });
    proxy_server.detach();

    // Give servers time to start
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 3. Client connects to Proxy requesting Mock Server
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(proxy_port); // Connect to Proxy
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Client connect to proxy failed");
        mock_running = false;
        // Wake up mock server accept() to exit
        int wake_sock = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in wake_addr;
        wake_addr.sin_family = AF_INET;
        wake_addr.sin_port = htons(mock_port);
        inet_pton(AF_INET, "127.0.0.1", &wake_addr.sin_addr);
        connect(wake_sock, (struct sockaddr *)&wake_addr, sizeof(wake_addr));
        close(wake_sock);

        mock_server.join();
        minitest::assert_true(false, "Could not connect to proxy");
    }

    // Send request to Proxy
    // Note: Request absolute URI because it's a proxy request
    std::string req = "GET http://localhost:" + std::to_string(mock_port) + "/ HTTP/1.1\r\n"
                      "Host: localhost:" + std::to_string(mock_port) + "\r\n"
                      "Connection: close\r\n\r\n";
    send(sock, req.c_str(), req.length(), 0);

    // Read response
    char buffer[4096] = {0};
    ssize_t valread = read(sock, buffer, 4096);
    close(sock);

    if (valread > 0) {
        std::string response(buffer, valread);
        minitest::assert_true(response.find("Hello, World!") != std::string::npos, "Did not receive expected response from upstream via proxy");
    } else {
        minitest::assert_true(false, "Failed to read response from proxy");
    }

    // 4. Cleanup
    mock_running = false;
    // Wake up mock server accept() to exit
    int wake_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in wake_addr;
    wake_addr.sin_family = AF_INET;
    wake_addr.sin_port = htons(mock_port);
    inet_pton(AF_INET, "127.0.0.1", &wake_addr.sin_addr);
    connect(wake_sock, (struct sockaddr *)&wake_addr, sizeof(wake_addr));
    close(wake_sock);

    if (mock_server.joinable()) {
        mock_server.join();
    }
}

// int main() {
//     minitest::run_test("Proxy Integration Test", test_proxy_integration);

//     std::cout << "-----------------------------------" << std::endl;
//     std::cout << "Tests Passed: " << minitest::g_passed << std::endl;
//     std::cout << "Tests Failed: " << minitest::g_failed << std::endl;

//     return minitest::g_failed == 0 ? 0 : 1;
// }
