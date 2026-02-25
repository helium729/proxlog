#include "connection_handler.h"
#include "http_message.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <string>
#include <sstream>

namespace proxlog {

void handle_client_connection(int client_fd) {
    HttpRequest req(client_fd);
    if (!req.isValid()) {
        close(client_fd);
        return;
    }

    std::string host = req.getHost();
    int port = req.getPort();

    if (host.empty()) {
        std::cerr << "Could not determine host." << std::endl;
        close(client_fd);
        return;
    }

    // Connect to upstream
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        close(client_fd);
        return;
    }

    struct addrinfo hints, *res;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);

    if (getaddrinfo(host.c_str(), port_str, &hints, &res) != 0) {
        std::cerr << "Host resolution failed for " << host << std::endl;
        close(client_fd);
        close(server_fd);
        return;
    }

    if (connect(server_fd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect failed");
        freeaddrinfo(res);
        close(client_fd);
        close(server_fd);
        return;
    }

    freeaddrinfo(res);

    if (req.getMethod() == "CONNECT") {
        const char *response = "HTTP/1.1 200 Connection Established\r\n\r\n";
        send(client_fd, response, strlen(response), 0);
    } else {
        // Rewrite request line (change absolute URI to path)
        std::string uri = req.getUri();
        std::string path = uri;
        if (uri.find("http://") == 0) {
             size_t host_start = 7;
             size_t path_start = uri.find('/', host_start);
             if (path_start != std::string::npos) {
                 path = uri.substr(path_start);
             } else {
                 path = "/";
             }
        }

        std::string new_request = req.getMethod() + " " + path + " " + req.getVersion() + "\r\n";

        std::istringstream stream(req.getRawHeaders());
        std::string line;
        std::getline(stream, line); // Skip request line

        while (std::getline(stream, line)) {
            // Check for empty line marking end of headers
            if (line.empty() || line == "\r") continue;

            std::string clean_line = line;
            if (!clean_line.empty() && clean_line.back() == '\r') clean_line.pop_back();

            if (clean_line.find("Proxy-Connection:") == 0) continue;
            if (clean_line.find("Connection:") == 0) continue;

            new_request += line + "\n";
        }
        new_request += "Connection: close\r\n\r\n";

        send(server_fd, new_request.c_str(), new_request.length(), 0);
    }

    // Send remaining body data if any
    std::string remaining = req.getRemainingData();
    if (!remaining.empty()) {
        send(server_fd, remaining.c_str(), remaining.length(), 0);
    }

    // Tunnel loop
    struct pollfd fds[2];
    fds[0].fd = client_fd;
    fds[0].events = POLLIN;
    fds[1].fd = server_fd;
    fds[1].events = POLLIN;

    char buffer[65536];

    while (true) {
        int ret = poll(fds, 2, -1);
        if (ret < 0) break;

        if (fds[0].revents & (POLLIN | POLLHUP)) {
            ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);
            if (n <= 0) break;
            send(server_fd, buffer, n, 0);
        }

        if (fds[1].revents & (POLLIN | POLLHUP)) {
            ssize_t n = recv(server_fd, buffer, sizeof(buffer), 0);
            if (n <= 0) break;
            send(client_fd, buffer, n, 0);
        }

        if ((fds[0].revents & (POLLERR | POLLNVAL)) || (fds[1].revents & (POLLERR | POLLNVAL))) {
            break;
        }
    }

    close(client_fd);
    close(server_fd);
}

} // namespace proxlog
