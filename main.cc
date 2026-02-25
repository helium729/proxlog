#include "server.h"

int main()
{
    proxlog::Server server(9990);
    server.run();
    return 0;
}
