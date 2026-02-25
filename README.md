# proxlog

A lightweight HTTP proxy server with logging capabilities written in C++.

## Overview

Proxlog is a simple yet efficient HTTP proxy server that intercepts and logs HTTP traffic. It supports both regular HTTP requests and CONNECT method for HTTPS tunneling. The proxy server is built with a thread pool for concurrent connection handling.

## Features

- **HTTP Proxy**: Forward HTTP requests and responses
- **HTTPS Support**: Handle CONNECT method for HTTPS tunneling
- **Thread Pool**: Efficient concurrent request handling
- **HTTP Parser**: Parse and validate HTTP requests
- **Logging**: Log all proxied traffic
- **Lightweight**: Minimal dependencies, pure C++

## Requirements

- CMake >= 3.5
- C++ compiler with C++11 support (GCC, Clang, etc.)
- pthread library
- Linux/Unix environment

## Building

1. Clone the repository:
   ```bash
   git clone https://github.com/helium729/proxlog.git
   cd proxlog
   ```

2. Create a build directory and compile:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

This will generate two executables:
- `proxlog`: The main proxy server executable
- `test_suite`: The test suite executable

## Usage

### Running the Proxy Server

Run the proxy server (default port is 9990):
```bash
./build/proxlog
```

The server will start listening on port 9990. You can then configure your browser or HTTP client to use `localhost:9990` as the proxy server.

### Configuring Your Browser

1. Open your browser's network settings
2. Set HTTP/HTTPS proxy to `localhost:9990`
3. All HTTP traffic will now be routed through proxlog

## Testing

Run the test suite to verify the installation:
```bash
./build/test_suite
```

The test suite includes:
- ThreadPool execution tests
- HTTP request parsing tests
- CONNECT method handling tests
- Integration tests with actual proxy server

## Project Structure

```
proxlog/
├── include/           # Header files
│   ├── server.h       # Main server class
│   ├── connection_handler.h
│   ├── http_message.h # HTTP request/response parsing
│   └── thread_pool.h  # Thread pool implementation
├── src/               # Implementation files
│   ├── server.cc
│   ├── connection_handler.cc
│   └── http_message.cc
├── tests/             # Test files
│   ├── minitest.h     # Custom testing framework
│   ├── minitest.cc
│   ├── test_main.cc
│   ├── test_threadpool.cc
│   ├── test_http_request.cc
│   └── test_integration.cc
├── main.cc            # Entry point
├── CMakeLists.txt     # CMake build configuration
└── README.md          # This file
```

## Architecture

### Components

- **Server**: Main proxy server that listens for incoming connections
- **ThreadPool**: Manages a pool of worker threads for handling connections
- **ConnectionHandler**: Handles individual client connections and proxying logic
- **HttpRequest**: Parses and validates HTTP requests

### How It Works

1. Server starts and listens on the configured port
2. When a client connects, the connection is handed to a worker thread from the pool
3. The worker thread parses the HTTP request
4. For regular HTTP requests, the proxy forwards the request to the target server
5. For CONNECT requests (HTTPS), the proxy establishes a tunnel between client and server
6. All traffic is logged for debugging and monitoring purposes

## License

This project is licensed under the Mozilla Public License Version 2.0. See the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Author

helium729
