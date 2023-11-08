#ifndef PROXLOG_PACKIN_H
#define PROXLOG_PACKIN_H

#include "pack.h"

namespace proxlog
{
    class PackIn : public Pack
    {
    public:
        PackIn();
        PackIn(std::string raw_data);
        virtual ~PackIn();

        std::string getRawData();
    private:
        std::string res_version;
        std::string res_status;
        std::map<std::string, std::string> res_headers;
        std::string res_body;
    };
}

#endif