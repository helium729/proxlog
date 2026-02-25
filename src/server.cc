#include "server.h"
#include "connection_handler.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <csignal>
#include <cstdlib>

namespace proxlog {

Server::Server(int port) : pool(16) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 100) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    
    // Ignore SIGPIPE
    signal(SIGPIPE, SIG_IGN);
    
    std::cout << "Server listening on port " << port << std::endl;
}

Server::~Server() {
    close(server_fd);
}

void Server::run() {
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);

        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        pool.enqueue([client_fd] {
            handle_client_connection(client_fd);
        });
    }
}

} // namespace proxlog
