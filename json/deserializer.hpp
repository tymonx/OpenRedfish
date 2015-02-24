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

class Deserializer : public Array {
public:
    Deserializer();
    Deserializer(const char* str);
    Deserializer(const String& str);

    Deserializer& operator<<(const String& str);
    Deserializer& operator>>(Value& value);

    struct Error {
        size_t line;
        size_t column;
        size_t offset;
        size_t size;
    };

    bool is_invalid() const { return m_pos < m_end; }
    Error get_error() const;
private:
    static constexpr size_t MAX_SIZE = 4096;

    const char* m_begin = nullptr;
    const char* m_pos = nullptr;
    const char* m_end = nullptr;
    size_t m_file_line = 1;
    size_t m_file_column = 1;
    size_t max_size = MAX_SIZE;
    void parsing();

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

    void prev_char() { --m_pos; --m_file_column; }
    void next_char() { ++m_pos; ++m_file_column; }
    void back_chars(size_t count) { m_pos -= count; m_file_column -= count; }
    void skip_chars(size_t count) { m_pos += count; m_file_column += count; }
    void next_newline() { ++m_pos; ++m_file_line; m_file_column = 0; }
    void reset_counts() { m_file_line = 1; m_file_column = 1; }

    char get_char() const { return *m_pos; }
    const char* get_position() const { return m_pos; }
    bool is_end() const { return m_pos >= m_end; }
    bool is_outbound(size_t offset) { return (m_pos + offset) > m_end; }
};

}

#endif /* _JSON_DESERIALIZER_HPP_ */
