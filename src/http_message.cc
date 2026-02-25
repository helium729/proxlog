#include "http_message.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstring>

namespace proxlog {

HttpRequest::HttpRequest(int socket_fd) : valid(false) {
    char buffer[4096];
    std::string raw;
    bool headers_complete = false;

    while (!headers_complete) {
        ssize_t bytes_read = recv(socket_fd, buffer, sizeof(buffer), 0);
        if (bytes_read <= 0) {
            // Connection closed or error
            return;
        }

        raw.append(buffer, bytes_read);

        size_t header_end = raw.find("\r\n\r\n");
        if (header_end != std::string::npos) {
            headers_complete = true;
            raw_headers = raw.substr(0, header_end + 4);
            remaining_data = raw.substr(header_end + 4);
            parse(raw_headers);
            valid = true;
        } else if (raw.length() > 65536) {
            // Header too large
            return;
        }
    }
}

void HttpRequest::parse(const std::string& data) {
    std::istringstream stream(data);
    std::string line;
    if (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        std::istringstream line_stream(line);
        line_stream >> method >> uri >> version;
    }

    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) break;

        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon + 1);

            // Trim whitespace
            while (!value.empty() && (value.front() == ' ' || value.front() == '\t')) value.erase(0, 1);
            while (!value.empty() && (value.back() == ' ' || value.back() == '\t')) value.pop_back();

            headers[key] = value;
        }
    }
}

std::string HttpRequest::getHeader(const std::string& name) const {
    // Header names are case-insensitive.
    // Ideally we store them lowercase or iterate.
    // For simplicity, let's iterate.
    for (auto const& [key, val] : headers) {
        std::string k = key;
        std::transform(k.begin(), k.end(), k.begin(), ::tolower);
        std::string n = name;
        std::transform(n.begin(), n.end(), n.begin(), ::tolower);
        if (k == n) return val;
    }
    return "";
}

std::string HttpRequest::getHost() const {
    if (method == "CONNECT") {
        // CONNECT host:port HTTP/1.1
        size_t colon = uri.find(':');
        if (colon != std::string::npos) {
            return uri.substr(0, colon);
        }
        return uri;
    }

    // Absolute URI: http://host:port/path
    if (uri.find("http://") == 0) {
        size_t host_start = 7;
        size_t path_start = uri.find('/', host_start);
        std::string host_port;
        if (path_start != std::string::npos) {
            host_port = uri.substr(host_start, path_start - host_start);
        } else {
            host_port = uri.substr(host_start);
        }

        size_t colon = host_port.find(':');
        if (colon != std::string::npos) {
            return host_port.substr(0, colon);
        }
        return host_port;
    }

    if (uri.find("https://") == 0) {
        size_t host_start = 8;
        size_t path_start = uri.find('/', host_start);
        std::string host_port;
        if (path_start != std::string::npos) {
            host_port = uri.substr(host_start, path_start - host_start);
        } else {
            host_port = uri.substr(host_start);
        }

        size_t colon = host_port.find(':');
        if (colon != std::string::npos) {
            return host_port.substr(0, colon);
        }
        return host_port;
    }

    // Relative URI, check Host header
    std::string host = getHeader("Host");
    size_t colon = host.find(':');
    if (colon != std::string::npos) {
        return host.substr(0, colon);
    }
    return host;
}

int HttpRequest::getPort() const {
    if (method == "CONNECT") {
        size_t colon = uri.find(':');
        if (colon != std::string::npos) {
            try {
                return std::stoi(uri.substr(colon + 1));
            } catch (...) {
                return 443;
            }
        }
        return 443; // Default for CONNECT? Or should we fail? Usually CONNECT is for HTTPS so 443.
    }

    // Check URI for port
    if (uri.find("http://") == 0 || uri.find("https://") == 0) {
        size_t scheme_end = uri.find("://");
        size_t host_start = scheme_end + 3;
        size_t path_start = uri.find('/', host_start);
        std::string host_port;
        if (path_start != std::string::npos) {
            host_port = uri.substr(host_start, path_start - host_start);
        } else {
            host_port = uri.substr(host_start);
        }

        size_t colon = host_port.find(':');
        if (colon != std::string::npos) {
            try {
                return std::stoi(host_port.substr(colon + 1));
            } catch (...) {
                return (uri.find("https://") == 0) ? 443 : 80;
            }
        }
        if (uri.find("https://") == 0) return 443;
        return 80;
    }

    // Check Host header
    std::string host = getHeader("Host");
    size_t colon = host.find(':');
    if (colon != std::string::npos) {
        try {
            return std::stoi(host.substr(colon + 1));
        } catch (...) {
            // Fallback to default if port is malformed
            return 80;
        }
    }

    // Default
    if (method == "CONNECT") return 443;
    return 80;
}

} // namespace proxlog
