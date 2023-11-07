#include "pack.h"

proxlog::Pack::Pack()
{
}

proxlog::Pack::Pack(std::string data)
{

}

proxlog::Pack::~Pack()
{
}

std::string proxlog::Pack::getData()
{
    std::string data = "";
    data += req_method + " " + req_path + " " + req_version + "\r\n";
    for (auto it = req_headers.begin(); it != req_headers.end(); ++it)
    {
        data += it->first + ": " + it->second + "\r\n";
    }
    data += "\r\n";
    data += req_body;
    return data;
}