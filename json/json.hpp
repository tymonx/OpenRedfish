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
 * @file json.hpp
 *
 * @brief JSON interface
 * */

#ifndef _JSON_HPP_
#define _JSON_HPP_

#include <limits>
#include <string>
#include <vector>
#include <utility>

namespace json {

class Value {
public:
    enum class Type {
        OBJECT,
        ARRAY,
        STRING,
        NUMBER,
        BOOLEAN,
        EMPTY
    };

    using KeyValue = std::pair<std::string, Value>;

    Value(Type type = Type::EMPTY);

    Value(std::nullptr_t);

    Value(bool boolean);

    Value(const char* str);

    Value(const std::string& str);

    Value(const KeyValue& key_value);

    Value(const std::string& key, const Value& value);

    Value(uint32_t value);

    Value(uint64_t value);

    Value(int32_t value);

    Value(int64_t value);

    Value(double value);

    Value(const Value& value) { operator=(value); }

    ~Value();

    Value& operator=(const Value& value);

    void push_back(const Value& value);

    size_t size() const;

    void clear();

    Value& operator[](const std::string& key);

    const Value& operator[](const std::string& key) const;

    Value& operator[](size_t index);

    const Value& operator[](size_t index) const;

    Type type() const { return m_type; }

    explicit operator const std::string&() const;

    explicit operator const char*() const;

    explicit operator bool() const;

    explicit operator std::nullptr_t() const;

    explicit operator int32_t() const;

    explicit operator int64_t() const;

    explicit operator uint32_t() const;

    explicit operator uint64_t() const;

    explicit operator double() const;

    bool operator==(Type type) const { return m_type == type; }

    bool operator==(const Value& value) const;

    bool operator==(const std::string& str) const;

    bool operator==(const char* str) const;

    bool operator==(bool boolean) const;

    bool operator==(std::nullptr_t empty) const;

    bool operator==(int32_t value) const;

    bool operator==(int64_t value) const;

    bool operator==(uint32_t value) const;

    bool operator==(uint64_t value) const;

    bool operator==(double value) const;

    bool operator!=(Type type) const { return m_type != type; }

    bool operator!=(const Value& value) const {
        return !operator==(value);
    }

    bool operator!=(const std::string& str) const {
        return !operator==(str);
    }

    bool operator!=(const char* str) const {
        return !operator==(str);
    }

    bool operator!=(bool boolean) const {
        return !operator==(boolean);
    }

    bool operator!=(std::nullptr_t empty) const {
        return !operator==(empty);
    }
private:
    class Number {
    public:
        enum class Type {
            INT,
            UINT,
            DOUBLE
        };

        Number() : m_type(Type::INT), m_int(0) { }

        Number(int32_t value) : m_type(Type::INT), m_int(value) { }

        Number(int64_t value) : m_type(Type::INT), m_int(value) { }

        Number(uint32_t value) : m_type(Type::UINT), m_uint(value) { }

        Number(uint64_t value) : m_type(Type::UINT), m_uint(value) { }

        Number(double value) : m_type(Type::DOUBLE), m_double(value) { }

        template<typename T> explicit operator T() const;

        operator double() const;

        bool operator==(Type type) const { return m_type == type; }

        Type type() const { return m_type; }
    private:
        enum Type m_type;

        union {
            int64_t m_int;
            uint64_t m_uint;
            double m_double;
        };
    };

    using Object = std::vector<KeyValue>;
    using Array = std::vector<Value>;

    union {
        Object m_object;
        Array m_array;
        std::string m_string;
        Number m_number;
        bool m_boolean;
    };

    enum Type m_type;

    void create_container(Type type);
    void assert_container() const;
    void assert_type(Type type) const;
};

} /* namespace json */

#endif /* _JSON_HPP_ */
