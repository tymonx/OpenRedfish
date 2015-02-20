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

#include "version.hpp"
#include "drawers.hpp"

#include <ctime>
#include <locale>

#include <iostream>

using std::cout;
using std::endl;
using std::time;
using std::time_t;
using std::strftime;
using std::localtime;
using namespace OpenRedfish::node;

static constexpr size_t TIME_BUFFER_SIZE = 26;

Version::Version(const string& name) : Node(name) {
    add_node(new Drawers);
}

void Version::get(Json::Value& response) {
    char time_buffer[TIME_BUFFER_SIZE];

    response["@odata.context"] = get_path() + "/$metadata#ServiceRoot";
    response["@odata.id"] = get_path();
    response["Name"] = "Root Service";

    time_t t = time(nullptr);
    if (strftime(time_buffer, TIME_BUFFER_SIZE, "%FT%T%z", localtime(&t))) {
        response["Time"] = time_buffer;
    } else {
        response["Time"] = Json::nullValue;
    }

    response["ServiceVersion"] = "0.94.0";
    response["Links"]["Drawers"]["@odata.id"] = get_node("Drawers")->get_path();
}

Version::~Version() { }
