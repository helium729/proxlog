#include "server.h"

proxlog::Server::Server(int port)
{
    // Create a socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options for TCP connections
    struct timeval timeout;
    timeout.tv_sec = 100;
    timeout.tv_usec = 0;
    if (setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, IPPROTO_TCP, TCP_NODELAY, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to a port
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) == -1) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connections and create a thread for each connection
    while (true) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        std::thread t(&proxlog::Server::handle_connection, this, client_fd);
        t.detach();
    }
}

void proxlog::Server::handle_connection(int client_fd) {
    // Handle the connection
    // ...
    // get request
    char buffer[65536] = {0};
    int valread = read(client_fd, buffer, 65536);
    printf("%s\n", buffer);

    // parse request
    std::string raw_data = buffer;
    proxlog::PackOut pack_out(raw_data);

    proxlog::PackOut fwd_pack_out = proxlog::procOut(pack_out);
    
    

    // send a http response
    // const char* msg =
    // "HTTP/1.1 200 OK\n"
    // "Content-Type: text/html\n"
    // "Content-Length: 12\n"
    // "\n"
    // "Hello world!";
    // send(client_fd, msg, strlen(msg), 0);

}

proxlog::Server::~Server()
{
}

