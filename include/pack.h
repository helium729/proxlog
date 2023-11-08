#ifndef PROXLOG_PACK_H
#define PROXLOG_PACK_H

#include <string>
#include <map>
#include <sstream>

namespace proxlog
{
    class Pack
    {
    public:
        Pack();
        Pack(std::string raw_data);
        virtual ~Pack();

        virtual std::string getRawData();
    private:      
    };
}

#endif