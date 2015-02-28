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

namespace json {

class Deserializer {
public:
    Deserializer();
    Deserializer(const char* str);
    Deserializer(const String& str);
    Deserializer(const Deserializer&) = default;
    Deserializer(Deserializer&&) = default;

    Deserializer& operator<<(const char* str);
    Deserializer& operator<<(const String& str);
    Deserializer& operator>>(Value& value);

    Deserializer& operator=(const Deserializer&) = default;
    Deserializer& operator=(Deserializer&&) = default;

    friend Deserializer operator>>(const char* str, Value& val);
    friend Deserializer operator>>(const String& str, Value& val);

    void set_limit(size_t limit = MAX_LIMIT_PER_OBJECT);

    bool empty() const;
    size_t size() const;

    struct Error {
        enum class Code {
            NONE,
            END_OF_FILE,
            MISS_VALUE,
            MISS_QUOTE,
            MISS_COMMA,
            MISS_COLON,
            MISS_CURLY_OPEN,
            MISS_CURLY_CLOSE,
            MISS_SQUARE_OPEN,
            MISS_SQUARE_CLOSE,
            NOT_MATCH_NULL,
            NOT_MATCH_TRUE,
            NOT_MATCH_FALSE,
            INVALID_ESCAPE,
            INVALID_UNICODE,
            INVALID_NUMBER_INTEGER,
            INVALID_NUMBER_FRACTION,
            INVALID_NUMBER_EXPONENT
        };

        Code code;
        size_t line;
        size_t column;
        size_t size;
        size_t offset;

        const char* decode();
    };

    bool is_invalid() const;
    Error get_error() const;
private:
    /*! Stack protection */
    static const size_t MAX_LIMIT_PER_OBJECT;

    Array m_array;

    const char* m_begin;
    const char* m_pos;
    const char* m_end;
    size_t m_limit;
    Error::Code m_error_code;

    void parsing();

    bool read_object_or_array(Value& value);
    bool read_object(Value& value);
    bool read_string(Value& value);
    bool read_value(Value& value);
    bool read_array(Value& value);
    bool read_curly_open();
    bool read_curly_close();
    bool read_square_open();
    bool read_square_close();
    bool read_comma();
    bool read_colon();
    bool read_quote();
    bool read_true(Value& value);
    bool read_false(Value& value);
    bool read_null(Value& value);
    bool read_number(Value& value);
    bool read_number_digit(String& str);
    bool read_number_integer(String& str);
    bool read_number_fractional(String& str);
    bool read_number_exponent(String& str);
    bool read_string_escape(String& str);
    bool read_string_escape_code(String& str);
    bool read_unicode(uint32_t& code);
    bool read_whitespaces();

    void prev_char();
    void next_char();
    void back_chars(size_t count);
    void skip_chars(size_t count);

    char get_char() const;
    const char* get_position() const;
    bool is_end() const;
    bool is_outbound(size_t offset);

    void clear_error();
    void set_error(Error::Code error_code);
};

Deserializer operator>>(const char* str, Value& val);
Deserializer operator>>(const String& str, Value& val);

}

#endif /* _JSON_DESERIALIZER_HPP_ */
