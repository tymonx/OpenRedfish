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

Testing microhttpd:

    build/http/microhttpd_example 4999
    curl -XGET localhost:4999
