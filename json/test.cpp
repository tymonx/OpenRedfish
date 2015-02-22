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

#include "json.hpp"

#include <iostream>

using namespace std;

int main(void) {

    json::Value val("Test");

    cout << std::string(val) << endl;

    val = json::Value(true);

    cout << bool(val) << endl;

    val = nullptr;

    cout << (val != nullptr) << endl;

    val = json::Value::Int(-1);

    cout << json::Value::Int(val.type()) << endl;
    cout << json::Value::Int(val) << endl;
    cout << (val == json::Value::Uint(2)) << endl;

    val = nullptr;

    val.append(nullptr)["test"] = json::Value::Uint(5);
    val.append(json::Value::Uint(6));
    val.append("dupa");
    val.append(val);

    cout << val.size() << endl;
    cout << int32_t(val[0]["test"].size()) << endl;

    for (const auto& data : val) {
        cout << "Foreach type: " << int(data.type()) << endl;
    }

    val = nullptr;

    for (const auto& data : val) {
        cout << "Foreach type: " << int(data.type()) << endl;
    }

    return 0;
}
