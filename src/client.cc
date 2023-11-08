#include "client.h"


proxlog::Client::Client(std::string host, int port)
{

}

proxlog::Client::~Client()
{

}

proxlog::PackIn proxlog::Client::Request(proxlog::PackOut pack, std::string host, int port)
{
    // create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Could not create socket");
        return proxlog::PackIn();
    }

    // set socket options
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        return proxlog::PackIn();
    }

    // set socket options for TCP connections
    struct timeval timeout;
    timeout.tv_sec = 100;
    timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    {
        perror("setsockopt failed");
        return proxlog::PackIn();
    }

    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    {
        perror("setsockopt failed");
        return proxlog::PackIn();
    }

    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&opt, sizeof(opt)) < 0)
    {
        perror("setsockopt failed");
        return proxlog::PackIn();
    }

    // DNS lookup
    struct hostent *he;
    struct in_addr **addr_list;
    if ((he = gethostbyname(host.c_str())) == NULL)
    {
        perror("gethostbyname failed");
        return proxlog::PackIn();
    }
    addr_list = (struct in_addr **)he->h_addr_list;

    // connect to remote server
    server.sin_addr = *addr_list[0];
    server.sin_family = AF_INET;
    server.sin_port = htons(80);
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("connect failed");
        return proxlog::PackIn();
    }

    // send request
    std::string request = pack.getRawData();
    if (send(sock, request.c_str(), request.length(), 0) < 0)
    {
        perror("send failed");
        return proxlog::PackIn();
    }

    // receive response
    char buffer[65536] = {0};
    int valread = read(sock, buffer, 65536);
    if (valread < 0)
    {
        perror("read failed");
        return proxlog::PackIn();
    }

    // parse response
    std::string raw_data = buffer;
    proxlog::PackIn pack_in(raw_data);
    
    return pack_in;    
}
