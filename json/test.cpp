/*!
 * @copyright
 * Copyright (c) 2015, Tymoteusz Blazejczyk
 *
 * @copyright
 * All rights reserved.
 *
 * @copyright
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * @copyright
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * @copyright
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * @copyright
 * * Neither the name of OpenRedfish nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * @copyright
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
 * */

#include "json.hpp"
#include <jsoncpp/json/json.h>
#include "serializer.hpp"

#include <iostream>

using namespace std;

int main(void) {

    json::Value val("Test");

    cout << std::string(val) << endl;

    val = json::Value(true);

    cout << bool(val) << endl;

    val = nullptr;

    cout << (val != nullptr) << endl;

    val = -1;

    cout << json::Value::Int(val.type()) << endl;
    cout << json::Value::Int(val) << endl;
    cout << (val == 2) << endl;

    val = nullptr;

    val.append(nullptr)["test"] = 5;
    val.append(6);
    val.append("Hello");
    val.append(val);

    cout << val.size() << endl;
    cout << int32_t(val[0]["test"].size()) << endl;

    for (const auto& data : val) {
        cout << "Foreach type: " << int(data.type()) << endl;
    }

    val = nullptr;
    val["key1"] = 4;
    val["key2"];
    val["key3"] = -2;
    val["key4"].append(json::Value::Pair("subtest1", 5));
    val["key4"].append(json::Value::Pair("subtest2", true));
    val["key5"]["inkey4"] = "Test1";
    val["key5"]["inkey5"] = "Test2";

    json::Serializer serializer;

    serializer << val << val << val;
    Json::Value test;
    cout << "Size: " << sizeof(json::Value) << endl;
    cout << "Size: " << sizeof(test) << endl;
    cout << "Serializer: " << json::Serializer(val) << endl;
    cout << "Serializer: " << serializer << endl;

    for (const auto& data : val) {
        cout << "Foreach type: " << int(data.type()) << endl;
    }

    return 0;
}
