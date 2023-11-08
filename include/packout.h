#ifndef PROXLOG_PACKOUT_H
#define PROXLOG_PACKOUT_H

#include "pack.h"

namespace proxlog
{
    class PackOut : public Pack
    {
    public:
        PackOut();
        PackOut(std::string raw_data);
        virtual ~PackOut();

        std::string getRawData();

        std::string getReqMethod();
        std::string getReqPath();
        std::string getReqVersion();
        std::map<std::string, std::string> getReqHeaders();
        std::string getReqBody();

        void setReqMethod(std::string req_method);
        void setReqPath(std::string req_path);

    private:      
        std::string req_method;
        std::string req_path;
        std::string req_version;
        std::map<std::string, std::string> req_headers;
        std::string req_body;
    };
}

#endif