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
 * @file deserializer.hpp
 *
 * @brief JSON deserializer interface
 * */

#ifndef _JSON_DESERIALIZER_HPP_
#define _JSON_DESERIALIZER_HPP_

#include "json.hpp"

#include <string>

namespace json {

class Deserializer {
public:
    Deserializer(const char* str);
    Deserializer(const std::string& str);

    Deserializer& operator>>(Value& value);
private:
    enum class ObjectState {
        SEARCHING_OPEN_BRACE,
        SEARCHING_STRING,
        SEARCHING_COLON,
        SEARCHING_VALUE,
        SEARCHING_COMMA
    };
    enum class StringState {
        SEARCHING_OPEN_QUOTE,
        SEARCHING_CHARACTER
    };
    enum class ArrayState {
        SEARCHING_OPEN_BRACKET,
        SEARCHING_COMMA,
        SEARCHING_VALUE
    };
    const char* m_pos;
    const char* m_end;
    Array m_array;
    bool read_object(Value& root);
    bool read_string(Value& root);
    bool read_value(Value& root);
    bool read_array(Value& root);
    bool read_number(Value& root);
    bool read_true(Value& root);
    bool read_false(Value& root);
    bool read_null(Value& root);
};

}

#endif /* JSON_DESERIALIZER_HPP_ */
