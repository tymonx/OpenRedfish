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
 *
 * @file serializer.hpp
 *
 * @brief JSON serializer interface
 * */

#ifndef _JSON_SERIALIZER_HPP_
#define _JSON_SERIALIZER_HPP_

#include "json.hpp"

#include <string>

namespace json {

class Serializer {
public:
    Serializer();

    Serializer(const Value& value);

    Serializer& operator<<(const Value& value);

    const char* c_str() const { return m_serialized.c_str(); }
    const std::string& str() const { return m_serialized; }

    explicit operator const char*() { return m_serialized.c_str(); }
    explicit operator const std::string&() { return m_serialized; }
private:
    std::string m_serialized;

    void write_members(const Value& value);
    void write_value(const Value& value);
    void write_array(const Value& value);
    void write_number(const Value& value);
    void write_string(const Value& value);
    void write_boolean(const Value& value);
    void write_empty(const Value& value);
};

inline std::ostream& operator<<(std::ostream& os,
        const Serializer& serializer) {
    return os << serializer.str();
}

}

#endif /* _JSON_SERIALIZER_HPP_ */
