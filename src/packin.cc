#include "packin.h"

proxlog::PackIn::PackIn(): Pack()
{
}

proxlog::PackIn::PackIn(std::string raw_data): Pack(raw_data)
{
}

proxlog::PackIn::~PackIn()
{
}

std::string proxlog::PackIn::getRawData()
{
    std::string raw_data = "";
    raw_data += res_version + " " + res_status + "\n";
    for (std::map<std::string, std::string>::iterator it = res_headers.begin(); it != res_headers.end(); ++it)
    {
        raw_data += it->first + ": " + it->second + "\n";
    }
    raw_data += "\n";
    raw_data += res_body;
    return raw_data;    
}