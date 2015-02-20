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

#include "node.hpp"

#include <exception>

using namespace OpenRedfish;

static const char ROOT[] = "";

Node::Node(const string& name) :
    m_name(name),
    m_back(nullptr),
    m_nodes() { }

void Node::get(Json::Value&) {
    throw std::exception();
}

void Node::del(Json::Value&) {
    throw std::exception();
}

void Node::patch(const Json::Value&, Json::Value&) {
    throw std::exception();
}

void Node::put(const Json::Value&, Json::Value&) {
    throw std::exception();
}

void Node::post(const Json::Value&, Json::Value&) {
    throw std::exception();
}

void Node::head(const Json::Value&, Json::Value&) {
    throw std::exception();
}

void Node::add_node(Node* node) {
    if (nullptr != node) {
        node->m_back = this;
        m_nodes[node->m_name].reset(node);
    }
}

void Node::erase(Node* node) {
    if (ROOT != node->m_name) {
        node->get_back()->m_nodes.erase(node->m_name);
    }
}

Node* Node::get_node(const string& path) {
    if (0 == path.size()) {
        return this;
    }

    Node* node = this;
    string node_path(path);

    if ('/' == node_path[0]) {
        node = get_root();
        node_path.erase(0, 1);
    }

    if (0 != node_path.size()) {
        if ('/' != node_path.back()) {
            node_path.push_back('/');
        }
    }

    string token;
    for (const auto c : node_path) {
        if ('/' == c) {
            node = node->m_nodes.at(token).get();
            token.clear();
        } else {
            token += c;
        }
    }

    return node;
}

Node* Node::get_root() {
    Node* node = this;
    while (ROOT != node->m_name) {
        node = node->m_back;
    }
    return node;
}

Node* Node::get_next() {
    return m_nodes.begin()->second.get();
}

string Node::get_path() const {
    string path;

    const Node* node = this;

    while (ROOT != node->m_name) {
        path = "/" + node->get_name() + path;
        node = node->m_back;
    }

    return path;
}

Node::~Node() { }
