#ifndef PROXLOG_CLIENT_H
#define PROXLOG_CLIENT_H

#include <string>
#include <sys/socket.h>
#include <pthread.h>
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <netinet/tcp.h>
#include "packin.h"
#include "packout.h"
#include <netdb.h>

namespace proxlog
{
    class Client
    {
    public:
        Client(std::string host, int port);
        ~Client();

        proxlog::PackIn Request(proxlog::PackOut pack, std::string host, int port);
    private:
        int sock;
        struct sockaddr_in server;
    };
}


#endif