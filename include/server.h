#ifndef PROXLOG_SERVER_H
#define PROXLOG_SERVER_H

#include "thread_pool.h"

namespace proxlog
{
    class Server
    {
    public:
        Server(int port);
        ~Server();
        void run();
    private:
        int server_fd;
        ThreadPool pool;
    };
}

#endif
