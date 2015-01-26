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

#include <string>
#include <cstdio>
#include <iostream>

using namespace std;
using namespace OpenRedfish::http;

static const char* DEFAULT_URL = "http://localhost:8888";

int main(int argc, char* argv[]) {

    string url = DEFAULT_URL;

    if (argc >= 2) {
        url = argv[1];
    }

    Server server(url);

    server.support([](Request&) {
            cout << "Unsupported method" << endl;
        });

    server.support(Method::GET, [](Request& message) {
            cout << "Method GET" << endl;
            message.reply(200, R"({"Test": "simple"})");
        });

    server.support(Method::POST, [](Request& message) {
            cout << "Method POST: " << message << endl;
        });

    server.open();

    cout << "Starting http server at address: '" << url << "'" << endl;
    cout << "Hit any key to exit..." << endl;

    getchar();

    server.close();

    return 0;
}
