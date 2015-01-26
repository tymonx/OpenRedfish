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

    build/bin/example_http_server
    curl -XGET localhost:8888
    curl -XPOST -H "Content-Type: application/json" localhost:8888 -d '{"Test": "test"}'

Developers
----------

Recommended flags for cmake:

    cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_FLAGS='-Werror -Weverything -Wno-padded -Wno-c++98-compat -Wno-c++98-compat-pedantic' ..

Issues
------

* Compiling with g++ -O0 -std=c++11 and linking with library that use pthread,
  running application after exit crush with segmentation fault (bug). Compile
  with optimization or with better compiler like clang++
