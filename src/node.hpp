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

#ifndef _OPEN_REDFISH_NODE_HPP_
#define _OPEN_REDFISH_NODE_HPP_

#include <jsoncpp/json/json.h>
#include <vector>
#include <string>
#include <memory>
#include <map>

namespace OpenRedfish {

using std::map;
using std::pair;
using std::vector;
using std::string;
using std::unique_ptr;

class Node {
public:
    Node(const string& name);
    virtual void get(Json::Value& result);
    virtual void del(Json::Value& response);
    virtual void post(const Json::Value& request, Json::Value& response);
    virtual void patch(const Json::Value& request, Json::Value& response);
    virtual void put(const Json::Value& request, Json::Value& response);
    virtual void head(const Json::Value& request, Json::Value& response);

    virtual ~Node();

    void add_node(Node* node);
    void erase(Node* node);
    Node* get_node(const string& name);
    Node* get_root();
    Node* get_next();
    inline Node* get_back() { return m_back; }
    inline size_t size() const { return m_nodes.size(); }
    string get_path() const;
    inline const string& get_name() const { return m_name; }

    class Iterator {
    public:
       Iterator(const map<string, unique_ptr<Node>>::iterator& it) :
           m_iterator(it) { }
       inline Node& operator*() const { return *(*m_iterator).second.get(); }
       inline Iterator& operator++() { m_iterator++; return *this; }
       inline bool operator!=(const Iterator& it) const {
           return m_iterator != it.m_iterator;
       }
    private:
       map<string, unique_ptr<Node>>::iterator m_iterator;
    };

    inline Iterator begin() { return {m_nodes.begin()}; }
    inline Iterator end() { return {m_nodes.end()}; }

    friend class Iterator;
private:
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    string m_name;
    Node* m_back;
    map<string, unique_ptr<Node>> m_nodes;
};

}

#endif /* _OPEN_REDFISH_NODE_HPP_ */
