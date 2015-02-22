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
 * @file serializer.cpp
 *
 * @brief JSON serializer implementation
 * */

#include "serializer.hpp"

using namespace json;

Serializer::Serializer() : m_serialized() { }

Serializer::Serializer(const Value& value) : m_serialized() {
    operator<<(value);
}

Serializer& Serializer::operator<<(const Value& value) {
    if ((Value::Type::MEMBERS == value.type()) ||
        (Value::Type::EMPTY == value.type())) {
        write_members(value);
    }

    return *this;
}

void Serializer::write_members(const Value& value) {
    m_serialized += '{';

    if (value.size() > 0) {
        for (const auto& pair : Members(value)) {
            write_string(pair.first);
            m_serialized += ":";
            write_value(pair.second);
            m_serialized += ",";
        };

        m_serialized.pop_back();
    }

    m_serialized += '}';
}

void Serializer::write_value(const Value& value) {
    switch (value.type()) {
    case Value::Type::MEMBERS:
        write_members(value);
        break;
    case Value::Type::ARRAY:
        write_array(value);
        break;
    case Value::Type::STRING:
        write_string(value);
        break;
    case Value::Type::NUMBER:
        write_number(value);
        break;
    case Value::Type::BOOLEAN:
        write_boolean(value);
        break;
    case Value::Type::EMPTY:
        write_empty(value);
        break;
    default:
        break;
    }
}

void Serializer::write_array(const Value& value) {
    m_serialized += '[';

    if (value.size() > 0) {
        for (const auto& val : value) {
            write_value(val);
            m_serialized += ",";
        }

        m_serialized.pop_back();
    }

    m_serialized += ']';
}

void Serializer::write_string(const Value& value) {
    m_serialized += "\"" + String(value) + "\"";
}

void Serializer::write_number(const Value& value) {
    switch (Number(value).type()) {
    case Number::Type::INT:
        m_serialized += std::to_string(Int(value));
        break;
    case Number::Type::UINT:
        m_serialized += std::to_string(Uint(value));
        break;
    case Number::Type::DOUBLE:
        m_serialized += std::to_string(Double(value));
        break;
    default:
        break;
    }
}

void Serializer::write_boolean(const Value& value) {
    m_serialized += (true == Bool(value)) ? "true" : "false";
}

void Serializer::write_empty(const Value&) {
    m_serialized += "null";
}
