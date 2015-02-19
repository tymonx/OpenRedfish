/*!
 * @copyright Copyright (c) 2015, Tymoteusz Blazejczyk
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of OpenRedfish nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "server.hpp"

using namespace OpenRedfish::http;

static void default_method(const Server::Request&,
        Server::Response&) { }

Server::Server(const string& url) :
    m_url(url),
    m_method_callback{
            default_method,
            default_method,
            default_method,
            default_method,
            default_method,
            default_method
            } { }

Server::~Server() { }

void Server::call(const Method method, const Request& request,
        Response& response) {
    if (method < MAX_METHODS) {
        m_method_callback[method](request, response);
    }
}

void Server::support(MethodCallback method_callback) {
    if (nullptr == method_callback) {
        method_callback = default_method;
    }

    for (size_t i = 0; i < MAX_METHODS; i++) {
        m_method_callback[i] = method_callback;
    }
}

void Server::support(const Method method,
        MethodCallback method_callback) {
    if (nullptr == method_callback) {
        method_callback = default_method;
    }

    if (method < MAX_METHODS) {
        m_method_callback[method] = method_callback;
    }
}

Server::Request::Request(const string& url, const string& message) :
    m_url(url),
    m_message(message) { }

Server::Response::Response() : m_status(404), m_message() { }

void Server::Response::set_reply(const unsigned int status,
        const string& message) {
    m_status = status;
    m_message = message;
}
