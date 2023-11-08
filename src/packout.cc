#include "packout.h"

proxlog::PackOut::PackOut(): Pack()
{
}

proxlog::PackOut::PackOut(std::string raw_data): Pack(raw_data)
{
    std::string line;
    std::istringstream iss(raw_data);
    std::getline(iss, line);
    std::istringstream iss_line(line);
    iss_line >> req_method >> req_path >> req_version;
    while (std::getline(iss, line))
    {
        if (line == "")
        {
            break;
        }
        std::istringstream iss_line(line);
        std::string key, value;
        std::getline(iss_line, key, ':');
        std::getline(iss_line, value);
        req_headers[key] = value;
    }
    std::getline(iss, req_body);
}

proxlog::PackOut::~PackOut()
{
}

std::string proxlog::PackOut::getRawData()
{
    std::string raw_data = "";
    raw_data += req_method + " " + req_path + " " + req_version + "\n";
    for (std::map<std::string, std::string>::iterator it = req_headers.begin(); it != req_headers.end(); ++it)
    {
        raw_data += it->first + ": " + it->second + "\n";
    }
    raw_data += "\n";
    raw_data += req_body;
    return raw_data;    
}

std::string proxlog::PackOut::getReqMethod()
{
    return req_method;
}

std::string proxlog::PackOut::getReqPath()
{
    return req_path;
}

std::string proxlog::PackOut::getReqVersion()
{
    return req_version;
}

std::map<std::string, std::string> proxlog::PackOut::getReqHeaders()
{
    return req_headers;
}

std::string proxlog::PackOut::getReqBody()
{
    return req_body;
}

void proxlog::PackOut::setReqMethod(std::string req_method)
{
    this->req_method = req_method;
}

void proxlog::PackOut::setReqPath(std::string req_path)
{
    this->req_path = req_path;
}