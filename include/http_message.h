#ifndef PROXLOG_HTTP_MESSAGE_H
#define PROXLOG_HTTP_MESSAGE_H

#include <string>
#include <map>
#include <vector>

namespace proxlog {

class HttpRequest {
public:
    HttpRequest(int socket_fd);

    bool isValid() const { return valid; }

    std::string getMethod() const { return method; }
    std::string getUri() const { return uri; }
    std::string getVersion() const { return version; }
    std::string getHeader(const std::string& name) const;

    // Returns host and updates port if found. If not found, returns empty string.
    std::string getHost() const;
    int getPort() const;

    std::string getRawHeaders() const { return raw_headers; }
    std::string getRemainingData() const { return remaining_data; }

private:
    std::string method;
    std::string uri;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string raw_headers;
    std::string remaining_data;
    bool valid = false;

    void parse(const std::string& data);
};

} // namespace proxlog

#endif
