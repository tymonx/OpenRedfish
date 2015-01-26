# OpenRedfish

Open implementation of Redfish RESTful specification.

Principal Goals & Scope
-----------------------

* BSD license
* Written in modern C++ using C++11 standard
* Fit for embedded devices that required small footprint
* Use only POSIX standard for maximum portability between systems
* Out of box library that include all necessary functionality

Building
--------

Create build directory, run cmake and make to build all sources:

    mkdir build
    cd build
    cmake ..
    make

Testing microhttpd (fixed port):

    build/http/example_http_server
    curl -XGET localhost:8888
    curl -XPOST -H "Content-Type: application/json" localhost:8888 -d '{"Test": "test"}'

Issues
------

* Compiling with g++ -O0 -std=c++11 and linking with library that use pthread,
  running application after exit crush with segmentation fault (bug). Compile
  with optimization or with better compiler like clang++
