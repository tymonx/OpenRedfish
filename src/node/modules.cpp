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

#include "modules.hpp"

#include <string>

using std::to_string;
using namespace OpenRedfish::node;

Modules::Modules() : Node("Modules") {
    add_node(new Modules::Module("my module", "1.0"));
    add_node(new Modules::Module("my module", "1.0"));
}

void Modules::get(Json::Value& response) {
    response["@odata.id"] = get_path();
    response["Name"] = "Modules Collection";

    response["Links"]["Members@odata.count"] = unsigned(size());
    response["Links"]["Members"] = Json::arrayValue;

    Json::Value member;
    for (const auto& child_node : *this) {
        member["@odata.id"] = child_node.get_path();
        response["Links"]["Members"].append(std::move(member));
    }
}

void Modules::post(const Json::Value& request, Json::Value& response) {
    Node* node = new Modules::Module(
            request["Name"].asString(),
            request["Version"].asString());
    add_node(node);
    node->get(response);
}

Modules::~Modules() { }

std::atomic_uint Modules::Module::id(1);

Modules::Module::Module(const string& name, const string& version) :
    Node(to_string(id++)),
    m_name(name),
    m_version(version) { }

void Modules::Module::get(Json::Value& response) {
    response["@odata.id"] = get_path();
    response["Id"] = get_name();
    response["Name"] = m_name;
    response["Version"] = m_version;
    response["Links"]["Drawer"]["@odata.id"] =
        get_back()->get_back()->get_path();
}

void Modules::Module::patch(const Json::Value& request, Json::Value& response) {
    if (request.isMember("Name")) {
        m_name = request["Name"].asString();
    }

    if (request.isMember("Version")) {
        m_version = request["Version"].asString();
    }
    get(response);
}

void Modules::Module::del(Json::Value& response) {
    get(response);
    erase(this);
}

Modules::Module::~Module() { }
