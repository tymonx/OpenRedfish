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

const size_t Serializer::DEFAULT_INDENT = 4;
const Serializer::Mode Serializer::DEFAULT_MODE = Serializer::Mode::COMPACT;

static constexpr char JSON_NULL[] = "null";
static constexpr char JSON_TRUE[] = "true";
static constexpr char JSON_FALSE[] = "false";
static constexpr char NEWLINE[] = "\n";

Serializer::Serializer(Mode mode) : m_level(0) {
    set_mode(mode);
}

Serializer::Serializer(const Value& value, Mode mode) : m_level(0) {
    set_mode(mode);
    operator<<(value);
}

Serializer& Serializer::operator<<(const Value& value) {
    m_level = 0;

    if (value.is_object() || value.is_null()) {
        write_object(value);
    }

    return *this;
}

std::ostream& json::operator<<(std::ostream& os,
        const Serializer& serializer) {
    return os << serializer.c_str();
}

void Serializer::set_mode(Mode mode) {
    switch (mode) {
    case Mode::COMPACT:
        m_enable_newline = false;
        m_indent = 0;
        m_colon_start = 1;
        m_colon_stop = 1;
        break;
    case Mode::PRETTY:
        m_enable_newline = true;
        m_indent = DEFAULT_INDENT;
        m_colon_start = 0;
        m_colon_stop = 3;
        break;
    default:
        break;
    }
}

void Serializer::enable_newline(bool enable) {
    m_enable_newline = enable;
}

void Serializer::set_indent(size_t indent) {
    m_indent = indent;
}

void Serializer::write_object(const Value& value) {
    if (value.size() > 0) {
        push_back('{');

        ++m_level;

        size_t indent_length = m_indent * m_level;

        for (const auto& pair : Object(value)) {
            append(NEWLINE, m_enable_newline);
            append(indent_length, ' ');
            write_string(pair.first);
            append(" : ", m_colon_start, m_colon_stop);
            write_value(pair.second);
            push_back(',');
        };

        pop_back();
        append(NEWLINE, m_enable_newline);

        --m_level;

        append(m_indent * m_level, ' ');
        push_back('}');
    } else {
        append("{}");
    }
}

void Serializer::write_value(const Value& value) {
    switch (value.type()) {
    case Value::Type::OBJECT:
        write_object(value);
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
    case Value::Type::NIL:
        write_empty(value);
        break;
    default:
        break;
    }
}

void Serializer::write_array(const Value& value) {
    if (value.size() > 0) {
        push_back('[');

        ++m_level;

        size_t indent_length = m_indent * m_level;

        for (const auto& val : value) {
            append(NEWLINE, m_enable_newline);
            append(indent_length, ' ');
            write_value(val);
            push_back(',');
        }

        pop_back();
        append(NEWLINE, m_enable_newline);

        --m_level;

        append(m_indent * m_level, ' ');
        push_back(']');
    } else {
        append("[]");
    }
}

void Serializer::write_string(const Value& value) {
    push_back('"');
    append(String(value));
    push_back('"');
}

void Serializer::write_number(const Value& value) {
    switch (Number(value).type()) {
    case Number::Type::INT:
        append(std::to_string(Int(value)));
        break;
    case Number::Type::UINT:
        append(std::to_string(Uint(value)));
        break;
    case Number::Type::DOUBLE:
        append(std::to_string(Double(value)));
        break;
    default:
        break;
    }
}

void Serializer::write_boolean(const Value& value) {
    append((true == Bool(value)) ? JSON_TRUE : JSON_FALSE);
}

void Serializer::write_empty(const Value&) {
    append(JSON_NULL);
}
