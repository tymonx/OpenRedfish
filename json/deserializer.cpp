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

#include <cmath>
#include <cstring>

using namespace json;
using Surrogate = std::pair<unsigned, unsigned>;

static constexpr char JSON_NULL[] = "null";
static constexpr char JSON_TRUE[] = "true";
static constexpr char JSON_FALSE[] = "false";
static constexpr size_t JSON_ESCAPE_HEX_DIGITS_SIZE = 6;
static constexpr Surrogate SURROGATE_MIN(0xD800, 0xDC00);
static constexpr Surrogate SURROGATE_MAX(0xDBFF, 0xDFFF);

template<size_t N>
constexpr size_t length(const char (&)[N]) { return (N - 1); }

Deserializer::Deserializer() :
    Array(), m_pos(nullptr), m_end(nullptr) { }

Deserializer::Deserializer(const char* str) :
    m_pos(str), m_end((nullptr == str) ? str : str + std::strlen(str)) {

    Value root;
    while (read_object(root)) {
        push_back(std::move(root));
    }
}

Deserializer::Deserializer(const std::string& str) :
    m_pos(str.data()), m_end(str.data() + str.size()) {

    Value root;
    while (read_object(root)) {
        push_back(std::move(root));
    }
}

Deserializer& Deserializer::operator<<(const std::string& str) {
    m_pos = str.data();
    m_end = str.data() + str.size();

    Value root;
    while (read_object(root)) {
        push_back(std::move(root));
    }

    return *this;
}

Deserializer& Deserializer::operator>>(Value& value) {
    if (empty()) {
        value = Value::Type::EMPTY;
    } else {
        value = std::move(back());
        pop_back();
    }

    return *this;
}

bool Deserializer::read_object(Value& value) {
    Value key;

    if (!read_curly_open()) { return false; }
    value = Value::Type::OBJECT;
    if (read_curly_close()) { return true; }

    do {
        if (!read_string(key)) { return false; }
        if (!read_colon()) { return false; }
        if (!read_value(value[String(key)])) { return false; }
        if (!read_comma()) { return read_curly_close(); }
    } while (true);
}

bool Deserializer::read_string(Value& value) {
    String str;

    if (!read_quote()) { return false; }

    while (m_pos < m_end) {
        if ('\\' == *m_pos) {
            m_pos++;
            if (!read_string_escape(str)) { return false; };
        } else if ('"' != *m_pos) {
            str.push_back(*(m_pos++));
        } else {
            value = str;
            ++m_pos;
            return true;
        }
    }

    return false;
}

bool Deserializer::read_string_escape(std::string& str) {
    bool ok = true;

    switch (*m_pos) {
    case '"':
    case '\\':
    case '/':
        str.push_back(*(m_pos++));
        break;
    case 'b':
        str.push_back('\b');
        ++m_pos;
        break;
    case 'f':
        str.push_back('\f');
        ++m_pos;
        break;
    case 'n':
        str.push_back('\n');
        ++m_pos;
        break;
    case 'r':
        str.push_back('\r');
        ++m_pos;
        break;
    case 't':
        str.push_back('\t');
        ++m_pos;
        break;
    case 'u':
        --m_pos;
        ok = read_string_escape_code(str);
        break;
    default:
        ok = false;
        break;
    }

    return ok;
}

bool Deserializer::read_string_escape_code(std::string& str) {
    Surrogate surrogate;
    uint32_t code;

    if (!read_unicode(code)) { return false; }
    if (read_unicode(surrogate.second)) {
        surrogate.first = code;
        if ((SURROGATE_MIN <= surrogate) && (surrogate <= SURROGATE_MAX)) {
            code = 0x10000 | ((0x3F & surrogate.first) << 10) | (0x3FF & surrogate.second);
        } else { m_pos -= JSON_ESCAPE_HEX_DIGITS_SIZE; }
    }

    if (code < 0x80) {
        str.push_back(char(code));
    } else if (code < 0x800) {
        str.push_back(0xC0 | char(0x1F & (code >> 6)));
        str.push_back(0x80 | char(0x3F & (code >> 0)));
    } else if (code < 0x10000) {
        str.push_back(0xE0 | char(0x0F & (code >> 12)));
        str.push_back(0x80 | char(0x3F & (code >> 6)));
        str.push_back(0x80 | char(0x3F & (code >> 0)));
    } else {
        str.push_back(0xF0 | char(0x07 & (code >> 18)));
        str.push_back(0x80 | char(0x3F & (code >> 12)));
        str.push_back(0x80 | char(0x3F & (code >> 6)));
        str.push_back(0x80 | char(0x3F & (code >> 0)));
    }

    return true;
}

bool Deserializer::read_unicode(uint32_t& code) {
    if ((m_pos + JSON_ESCAPE_HEX_DIGITS_SIZE) > m_end) { return false; }
    if ('\\' != m_pos[0]) { return false; }
    if ('u' != m_pos[1]) { return false; }

    for (size_t i = 2; i < JSON_ESCAPE_HEX_DIGITS_SIZE; i++) {
        if (!std::isxdigit(m_pos[i])) { return false; }
    }

    size_t pos;
    code = unsigned(std::stoul(&m_pos[2], &pos, 16));

    m_pos += JSON_ESCAPE_HEX_DIGITS_SIZE;

    return true;
}

bool Deserializer::read_value(Value& value) {
    using std::isdigit;

    bool ok = false;

    if (!read_whitespaces()) { return false; }

    switch (*m_pos) {
    case '"':
        ok = read_string(value);
        break;
    case '{':
        ok = read_object(value);
        break;
    case '[':
        ok = read_array(value);
        break;
    case 't':
        ok = read_true(value);
        break;
    case 'f':
        ok = read_false(value);
        break;
    case 'n':
        ok = read_null(value);
        break;
    case '-':
        ok = read_number(value);
        break;
    default:
        if (isdigit(*m_pos)) {
            ok = read_number(value);
        }
        break;
    }

    return ok;
}

bool Deserializer::read_array(Value& value) {
    Value array_value;

    if (!read_square_open()) { return false; }
    value = Value::Type::ARRAY;
    if (read_square_close()) { return true; }

    do {
        if (!read_value(array_value)) { return false; }
        value.push_back(std::move(array_value));
        if (!read_comma()) { return read_square_close(); }
    } while (true);
}

bool Deserializer::read_colon() {
    if (!read_whitespaces()) { return false; }
    if (':' != *m_pos) { return false; }
    ++m_pos;
    return true;
}

bool Deserializer::read_quote() {
    if (!read_whitespaces()) { return false; }
    if ('"' != *m_pos) { return false; }
    ++m_pos;
    return true;
}

bool Deserializer::read_curly_open() {
    if (!read_whitespaces()) { return false; }
    if ('{' != *m_pos) { return false; }
    ++m_pos;
    return true;
}

bool Deserializer::read_curly_close() {
    if (!read_whitespaces()) { return false; }
    if ('}' != *m_pos) { return false; }
    ++m_pos;
    return true;
}

bool Deserializer::read_square_open() {
    if (!read_whitespaces()) { return false; }
    if ('[' != *m_pos) { return false; }
    ++m_pos;
    return true;
}

bool Deserializer::read_square_close() {
    if (!read_whitespaces()) { return false; }
    if (']' != *m_pos) { return false; }
    ++m_pos;
    return true;
}

bool Deserializer::read_comma() {
    if (!read_whitespaces()) { return false; }
    if (',' != *m_pos) { return false; }
    ++m_pos;
    return true;
}

bool Deserializer::read_whitespaces() {
    while (m_pos < m_end) {
        switch (*m_pos) {
        case ' ':
        case '\n':
        case '\r':
        case '\t':
            ++m_pos;
            break;
        default:
            return true;
        }
    }

    return false;
}

bool Deserializer::read_number_digit(std::string& str) {
    bool ok = false;

    while (m_pos < m_end) {
        if (std::isdigit(*m_pos)) {
            str.push_back(*(m_pos++));
            ok = true;
        } else { return ok; }
    }

    return ok;
}

bool Deserializer::read_number_integer(std::string& str) {
    if (m_pos >= m_end) { return false; }

    if ('-' == *m_pos) {
        str.push_back(*(m_pos++));
        if (m_pos >= m_end) { return false; }
    }

    if ('0' == *m_pos) {
        str.push_back(*(m_pos++));
        return true;
    }

    return read_number_digit(str);
}

bool Deserializer::read_number_fractional(std::string& str) {
    if (m_pos >= m_end) { return false; }

    bool ok = true;

    if ('.' == *m_pos) {
        str += "0.";
        ++m_pos;
        ok = read_number_digit(str);
    }

    return ok;
}

bool Deserializer::read_number_exponent(std::string& str) {
    if (m_pos >= m_end) { return false; }

    if (('e' != *m_pos) && ('E' != *m_pos)) { return true; }

    ++m_pos;
    if (m_pos >= m_end) { return false; }

    if (('+' == *m_pos) || ('-' == *m_pos)) {
        str.push_back(*(m_pos++));
    }

    return read_number_digit(str);
}

bool Deserializer::read_number(Value& value) {
    using std::pow;
    using std::stol;
    using std::stod;
    using std::signbit;

    std::string str_integer;
    std::string str_fractional;
    std::string str_exponent;

    read_whitespaces();
    if (!read_number_integer(str_integer)) { return false; }
    if (!read_number_fractional(str_fractional)) { return false; }
    if (!read_number_exponent(str_exponent)) { return false; }

    long int integer = stol(str_integer);
    double fractional = str_fractional.empty() ? 0.0 : stod(str_fractional);
    long int exponent = str_exponent.empty() ? 0 : stol(str_exponent);

    if (str_fractional.empty()) {
        if (signbit(exponent)) {
            value = Double(integer * pow(10, exponent));
        } else {
            integer *= pow(10, exponent);
            if (signbit(integer)) { value = Int(integer); }
            else { value = Uint(integer); }
        }
    } else {
        fractional = ('-' == str_integer[0]) ? -fractional : fractional;
        value = Double((integer + fractional) * pow(10, exponent));
    }

    return true;
}

bool Deserializer::read_true(Value& value) {
    if ((m_pos + length(JSON_TRUE)) > m_end) { return false; }

    if (0 != std::strncmp(m_pos, JSON_TRUE, length(JSON_TRUE))) {
        return false;
    }

    m_pos += length(JSON_TRUE);
    value = true;
    return true;
}

bool Deserializer::read_false(Value& value) {
    if ((m_pos + length(JSON_FALSE)) > m_end) { return false; }

    if (0 != std::strncmp(m_pos, JSON_FALSE, length(JSON_FALSE))) {
        return false;
    }

    m_pos += length(JSON_FALSE);
    value = false;
    return true;
}

bool Deserializer::read_null(Value& value) {
    if ((m_pos + length(JSON_NULL)) > m_end) { return false; }

    if (0 != std::strncmp(m_pos, JSON_NULL, length(JSON_NULL))) {
        return false;
    }

    m_pos += length(JSON_NULL);
    value = nullptr;
    return true;
}
