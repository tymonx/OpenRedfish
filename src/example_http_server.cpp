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

#include "node/root.hpp"

#include "jsoncpp/json/reader.h"
#include "microhttpd.hpp"

#include <string>
#include <cstdio>
#include <iostream>

using namespace std;
using namespace OpenRedfish;
using namespace OpenRedfish::http;

static const char* DEFAULT_URL = "http://localhost:8888";

int main(int argc, char* argv[]) {

    string url = DEFAULT_URL;

    if (argc >= 2) {
        url = argv[1];
    }

    node::Root root;

    Server& server = *new MicroHttpd(url);

    server.support([](const Server::Request&, Server::Response& response) {
            cout << "Unsupported method" << endl;
            response.set_reply(404);
        });

    server.support(Server::Method::GET,
            [&root](const Server::Request& request, Server::Response& response) {

            try {
                Node* node = root.get_node(request.get_url());

                Json::Value json_response{};

                node->get(json_response);

                response.set_reply(200, json_response.toStyledString());
            } catch (...) {
                response.set_reply(405);
            }
        });

    server.support(Server::Method::DELETE,
            [&root](const Server::Request& request, Server::Response& response) {

            try {
                Node* node = root.get_node(request.get_url());

                Json::Value json_response{};

                node->del(json_response);

                response.set_reply(200, json_response.toStyledString());
            } catch (...) {
                response.set_reply(405);
            }
        });

    server.support(Server::Method::POST,
            [&root](const Server::Request& request, Server::Response& response) {
            try {
                Node* node = root.get_node(request.get_url());

                Json::Value json_request{};
                Json::Value json_response{};
                Json::Reader reader;
                reader.parse(request.get_message(), json_request);

                node->post(json_request, json_response);

                response.set_reply(200, json_response.toStyledString());
            } catch (...) {
                response.set_reply(405);
            }
        });

    server.support(Server::Method::PATCH,
            [&root](const Server::Request& request, Server::Response& response) {
            try {
                Node* node = root.get_node(request.get_url());

                Json::Value json_request{};
                Json::Value json_response{};
                Json::Reader reader;
                reader.parse(request.get_message(), json_request);

                node->patch(json_request, json_response);

                response.set_reply(200, json_response.toStyledString());
            } catch (...) {
                response.set_reply(405);
            }
        });

    server.open();

    cout << "Starting http server at address: '" << url << "'" << endl;
    cout << "Hit any key to exit..." << endl;

    getchar();

    server.close();

    delete &server;

    return 0;
}
