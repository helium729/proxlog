#ifndef PROXLOG_PACK_H
#define PROXLOG_PACK_H

#include <string>
#include <map>

namespace proxlog
{
    class Pack
    {
    public:
        Pack();
        Pack(std::string data);
        ~Pack();

        std::string getData();
    private:
        std::string req_method;
        std::string req_path;
        std::string req_version;
        std::map<std::string, std::string> req_headers;
        std::string req_body;        
    };
}

#endif