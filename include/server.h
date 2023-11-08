#ifndef PROXLOG_SERVER_H
#define PROXLOG_SERVER_H

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

#include "packout.h"
#include "packin.h"
#include "proc.h"

namespace proxlog
{
    class Server
    {
    public:
        Server(int port);
        ~Server();
    private:
        void handle_connection(int client_fd);
    };
}

#endif