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
 * @file deserializer.cpp
 *
 * @brief JSON deserializer implementation
 * */

#include "deserializer.hpp"

#include <cstring>
#include <iostream>

using namespace json;

Deserializer::Deserializer(const char* str) :
    m_pos(str), m_end((nullptr == str) ? str : str + std::strlen(str)) {

    Value root;
    while (read_object(root)) {
        m_array.push_back(std::move(root));
    }
}

Deserializer::Deserializer(const std::string& str) :
    m_pos(str.data()), m_end(str.data() + str.size()) {

    Value root;
    while (read_object(root)) {
        m_array.push_back(std::move(root));
    }
}

Deserializer& Deserializer::operator>>(Value& value) {
    if (m_array.empty()) {
        value = std::move(Value());
    } else {
        value = std::move(m_array.back());
        m_array.pop_back();
    }

    return *this;
}

bool Deserializer::read_object(Value& value) {
    using State = ObjectState;

    bool ok = false;
    bool parsing = true;
    Value key;
    State state = State::SEARCHING_OPEN_BRACE;

    while ((m_pos < m_end) && parsing) {
        switch (*m_pos) {
        case '{':
            if (State::SEARCHING_OPEN_BRACE == state) {
                m_pos++;
                state = State::SEARCHING_STRING;
            } else { parsing = false; }
            break;
        case '}':
            if ((State::SEARCHING_COMMA == state)
             || (State::SEARCHING_STRING == state)) {
                m_pos++;
                ok = true;
            }
            parsing = false;
            break;
        case ':':
            if (State::SEARCHING_COLON == state) {
                m_pos++;
                state = State::SEARCHING_VALUE;
            } else { parsing = false; }
            break;
        case ',':
            if (State::SEARCHING_COMMA == state) {
                m_pos++;
                state = State::SEARCHING_STRING;
            } else { parsing = false; }
            break;
        case '\r':
        case '\n':
        case '\t':
        case ' ':
            m_pos++;
            break;
        default:
            if (State::SEARCHING_STRING == state) {
                if (!read_string(key)) { parsing = false; }
                state = State::SEARCHING_COLON;
            } else if (State::SEARCHING_VALUE == state) {
                if (!read_value(value[String(key)])) { parsing = false; }
                state = State::SEARCHING_COMMA;
            } else { parsing = false; }
            break;
        }
    }

    return ok;
}

bool Deserializer::read_string(Value& value) {
    using State = StringState;

    bool ok = false;
    bool parsing = true;
    String str;
    State state = State::SEARCHING_OPEN_QUOTE;

    while ((m_pos < m_end) && parsing) {
        switch (*m_pos) {
        case '"':
            if (State::SEARCHING_OPEN_QUOTE == state) {
                state = State::SEARCHING_CHARACTER;
            } else {
                value = std::move(str);
                ok = true;
                parsing = false;
            }
            m_pos++;
            break;
        case '\r':
        case '\n':
        case '\t':
        case ' ':
            if (State::SEARCHING_CHARACTER == state) {
                str += *m_pos;
            }
            m_pos++;
            break;
        default:
            if (State::SEARCHING_CHARACTER == state) {
                str += *m_pos;
                m_pos++;
            } else { parsing = false; }
            break;
        }
    }

    return ok;
}

bool Deserializer::read_value(Value& value) {
    bool ok = false;
    bool parsing = true;

    while ((m_pos < m_end) && parsing) {
        switch (*m_pos) {
        case '"':
            ok = read_string(value);
            parsing = false;
            break;
        case '{':
            ok = read_object(value);
            parsing = false;
            break;
        case '[':
            ok = read_array(value);
            parsing = false;
            break;
        case 't':
            ok = read_true(value);
            parsing = false;
            break;
        case 'f':
            ok = read_false(value);
            parsing = false;
            break;
        case 'n':
            ok = read_null(value);
            parsing = false;
            break;
        case '\r':
        case '\n':
        case '\t':
        case ' ':
            m_pos++;
            break;
        default:
            if (std::isdigit(*m_pos) || ('-'== *m_pos)) {
                ok = read_number(value);
            }
            parsing = false;
            break;
        }
    }

    return ok;
}

bool Deserializer::read_array(Value& value) {
    using State = ArrayState;

    bool ok = false;
    bool parsing = true;
    Value array_value;
    State state = State::SEARCHING_OPEN_BRACKET;

    while ((m_pos < m_end) && parsing) {
        switch (*m_pos) {
        case '[':
            if (State::SEARCHING_OPEN_BRACKET == state) {
                m_pos++;
                state = State::SEARCHING_VALUE;
            } else { parsing = false; }
            break;
        case ',':
            if (State::SEARCHING_COMMA == state) {
                m_pos++;
                state = State::SEARCHING_VALUE;
            } else { parsing = false; }
            break;
        case ']':
            if ((State::SEARCHING_COMMA == state)
             || (State::SEARCHING_VALUE == state)) {
                m_pos++;
                ok = true;
            }
            parsing = false;
            break;
        case '\r':
        case '\n':
        case '\t':
        case ' ':
            m_pos++;
            break;
        default:
            if (State::SEARCHING_VALUE == state) {
                if (read_value(array_value)) {
                    value.push_back(std::move(array_value));
                } else { parsing= false; }
                state = State::SEARCHING_COMMA;
            } else { parsing = false; }
            break;
        }
    }

    return ok;
}

bool Deserializer::read_number(Value& value) {
    (void)value;

    return false;
}

bool Deserializer::read_true(Value& value) {
    if ((m_end - m_pos) < 4) return false;

    if (!std::strncmp(m_pos, "true", 4)) {
        m_pos += 4;
        value = true;
        return true;
    }

    return false;
}

bool Deserializer::read_false(Value& value) {
    if ((m_end - m_pos) < 5) return false;

    if (!std::strncmp(m_pos, "false", 5)) {
        m_pos += 5;
        value = false;
        return true;
    }

    return false;
}

bool Deserializer::read_null(Value& value) {
    if ((m_end - m_pos) < 4) return false;

    if (!std::strncmp(m_pos, "null", 4)) {
        m_pos += 4;
        value = nullptr;
        return true;
    }

    return false;
}
