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
static constexpr size_t ESCAPE_HEX_DIGITS_SIZE = 6;
static constexpr Surrogate SURROGATE_MIN(0xD800, 0xDC00);
static constexpr Surrogate SURROGATE_MAX(0xDBFF, 0xDFFF);

template<size_t N>
constexpr size_t length(const char (&)[N]) { return (N - 1); }

Deserializer::Deserializer() { }

Deserializer::Deserializer(const char* str) :
    m_begin(str),
    m_pos(m_begin),
    m_end((nullptr == m_begin) ? nullptr : m_begin + std::strlen(str)) {

    parsing();
}

Deserializer::Deserializer(const String& str) :
    m_begin(str.data()),
    m_pos(m_begin),
    m_end(m_begin + str.size()) {

    parsing();
}

Deserializer& Deserializer::operator<<(const String& str) {
    reset_counts();

    m_begin = str.data();
    m_pos = m_begin;
    m_end = m_begin + str.size();

    parsing();

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

void Deserializer::parsing() {
    Value root;
    while (read_object(root)) {
        push_back(std::move(root));
    }
}

Deserializer::Error Deserializer::get_error() const {
    Error error;

    error.line = m_file_line;
    error.column = m_file_column;
    error.offset = size_t(m_pos - m_begin);
    error.size = size_t(m_end - m_begin);

    return error;
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

    while (!is_end()) {
        if ('\\' == get_char()) {
            next_char();
            if (!read_string_escape(str)) { return false; };
        } else if ('"' != get_char()) {
            str.push_back(get_char());
            next_char();
        } else {
            value = str;
            next_char();
            return true;
        }
    }

    return false;
}

bool Deserializer::read_string_escape(String& str) {
    bool ok = true;

    switch (get_char()) {
    case '"':
    case '\\':
    case '/':
        str.push_back(get_char());
        next_char();
        break;
    case 'b':
        str.push_back('\b');
        next_char();
        break;
    case 'f':
        str.push_back('\f');
        next_char();
        break;
    case 'n':
        str.push_back('\n');
        next_char();
        break;
    case 'r':
        str.push_back('\r');
        next_char();
        break;
    case 't':
        str.push_back('\t');
        next_char();
        break;
    case 'u':
        prev_char();
        ok = read_string_escape_code(str);
        break;
    default:
        ok = false;
        break;
    }

    return ok;
}

bool Deserializer::read_string_escape_code(String& str) {
    Surrogate surrogate;
    uint32_t code;

    if (!read_unicode(code)) { return false; }
    if (read_unicode(surrogate.second)) {
        surrogate.first = code;
        if ((SURROGATE_MIN <= surrogate) && (surrogate <= SURROGATE_MAX)) {
            code = 0x10000 | ((0x3F & surrogate.first) << 10) | (0x3FF & surrogate.second);
        } else { back_chars(ESCAPE_HEX_DIGITS_SIZE); }
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
    if (is_outbound(ESCAPE_HEX_DIGITS_SIZE)) { return false; }

    const char* ch = get_position();
    if ('\\' != ch[0]) { return false; }
    if ('u'  != ch[1]) { return false; }

    for (size_t i = 2; i < ESCAPE_HEX_DIGITS_SIZE; i++) {
        if (!std::isxdigit(ch[i])) { return false; }
    }

    size_t unused;
    code = unsigned(std::stoul(ch + 2, &unused, 16));

    skip_chars(ESCAPE_HEX_DIGITS_SIZE);

    return true;
}

bool Deserializer::read_value(Value& value) {
    bool ok = false;

    if (!read_whitespaces()) { return false; }

    switch (get_char()) {
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
        if (std::isdigit(get_char())) {
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
    if (':' != get_char()) { return false; }
    next_char();
    return true;
}

bool Deserializer::read_quote() {
    if (!read_whitespaces()) { return false; }
    if ('"' != get_char()) { return false; }
    next_char();
    return true;
}

bool Deserializer::read_curly_open() {
    if (!read_whitespaces()) { return false; }
    if ('{' != get_char()) { return false; }
    next_char();
    return true;
}

bool Deserializer::read_curly_close() {
    if (!read_whitespaces()) { return false; }
    if ('}' != get_char()) { return false; }
    next_char();
    return true;
}

bool Deserializer::read_square_open() {
    if (!read_whitespaces()) { return false; }
    if ('[' != get_char()) { return false; }
    next_char();
    return true;
}

bool Deserializer::read_square_close() {
    if (!read_whitespaces()) { return false; }
    if (']' != get_char()) { return false; }
    next_char();
    return true;
}

bool Deserializer::read_comma() {
    if (!read_whitespaces()) { return false; }
    if (',' != get_char()) { return false; }
    next_char();
    return true;
}

bool Deserializer::read_whitespaces() {
    while (!is_end()) {
        switch (get_char()) {
        case '\n':
            next_newline();
            break;
        case ' ':
        case '\r':
        case '\t':
            next_char();
            break;
        default:
            return true;
        }
    }

    return false;
}

bool Deserializer::read_number_digit(String& str) {
    bool ok = false;

    while (!is_end()) {
        if (std::isdigit(get_char())) {
            str.push_back(get_char());
            next_char();
            ok = true;
        } else { return ok; }
    }

    return ok;
}

bool Deserializer::read_number_integer(String& str) {
    if (is_end()) { return false; }

    if ('-' == get_char()) {
        str.push_back(get_char());
        next_char();
        if (is_end()) { return false; }
    }

    if ('0' == get_char()) {
        str.push_back(get_char());
        next_char();
        return true;
    }

    return read_number_digit(str);
}

bool Deserializer::read_number_fractional(String& str) {
    if (is_end()) { return false; }

    bool ok = true;

    if ('.' == get_char()) {
        str += "0.";
        next_char();
        ok = read_number_digit(str);
    }

    return ok;
}

bool Deserializer::read_number_exponent(String& str) {
    if (is_end()) { return false; }

    if (('e' != get_char()) && ('E' != get_char())) { return true; }

    next_char();
    if (is_end()) { return false; }

    if (('+' == get_char()) || ('-' == get_char())) {
        str.push_back(get_char());
        next_char();
    }

    return read_number_digit(str);
}

bool Deserializer::read_number(Value& value) {
    using std::pow;
    using std::stol;
    using std::stod;
    using std::signbit;

    String str_integer;
    String str_fractional;
    String str_exponent;

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
    if (is_outbound(length(JSON_TRUE))) { return false; }

    if (0 != std::strncmp(get_position(), JSON_TRUE, length(JSON_TRUE))) {
        return false;
    }

    value = true;

    skip_chars(length(JSON_TRUE));
    return true;
}

bool Deserializer::read_false(Value& value) {
    if (is_outbound(length(JSON_FALSE))) { return false; }

    if (0 != std::strncmp(get_position(), JSON_FALSE, length(JSON_FALSE))) {
        return false;
    }

    value = false;

    skip_chars(length(JSON_FALSE));
    return true;
}

bool Deserializer::read_null(Value& value) {
    if (is_outbound(length(JSON_NULL))) { return false; }

    if (0 != std::strncmp(get_position(), JSON_NULL, length(JSON_NULL))) {
        return false;
    }

    value = nullptr;

    skip_chars(length(JSON_NULL));
    return true;
}
