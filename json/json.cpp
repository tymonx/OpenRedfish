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
 * @file json.cpp
 *
 * @brief JSON implementation
 * */

#include "json.hpp"

#include <cmath>
#include <stdexcept>

using namespace json;

template<typename T> Value::Number::operator T() const {
    T value;

    switch(m_type) {
    case Type::INT:
        value = T(m_int);
        break;
    case Type::UINT:
        value = T(m_uint);
        break;
    case Type::DOUBLE:
        value = T(std::round(m_double));
        break;
    default:
        value = 0;
        break;
    }

    return value;
}

template Value::Number::operator int32_t() const;
template Value::Number::operator int64_t() const;
template Value::Number::operator uint32_t() const;
template Value::Number::operator uint64_t() const;

Value::Number::operator double() const {
    double value;

    switch(m_type) {
    case Type::INT:
        value = m_int;
        break;
    case Type::UINT:
        value = m_uint;
        break;
    case Type::DOUBLE:
        value = m_double;
        break;
    default:
        value = 0.0;
        break;
    }

    return value;
}

Value::Value(Type type) : m_type(type) {
    create_container(m_type);
}

Value::Value(std::nullptr_t) : m_type(Type::EMPTY) { }

Value::Value(bool boolean) : m_type(Type::BOOLEAN) {
    m_boolean = boolean;
}

Value::Value(const char* str) : m_type(Type::STRING) {
    new (&m_string) std::string(str);
}

Value::Value(const std::string& str) : m_type(Type::STRING) {
    new (&m_string) std::string(str);
}

Value::Value(const KeyValue& key_value) : m_type(Type::OBJECT) {
    new (&m_object) Object{key_value};
}

Value::Value(const std::string& key, const Value& value) :
    m_type(Type::OBJECT) {
    new (&m_object) Object{std::make_pair(key, value)};
}

Value::Value(uint32_t value) : m_type(Type::NUMBER) {
    new (&m_number) Number(value);
}

Value::Value(uint64_t value) : m_type(Type::NUMBER) {
    new (&m_number) Number(value);
}

Value::Value(int32_t value) : m_type(Type::NUMBER) {
    new (&m_number) Number(value);
}

Value::Value(int64_t value) : m_type(Type::NUMBER) {
    new (&m_number) Number(value);
}

Value::Value(double value) : m_type(Type::NUMBER) {
    new (&m_number) Number(value);
}

Value::~Value() {
    switch(m_type) {
    case Type::OBJECT:
        m_object.~vector();
        break;
    case Type::ARRAY:
        m_array.~vector();
        break;
    case Type::STRING:
        m_string.~basic_string();
        break;
    case Type::NUMBER:
        m_number.~Number();
        break;
    case Type::BOOLEAN:
    case Type::EMPTY:
    default:
        break;
    }
}

void Value::assert_container() const {
    if ((Type::OBJECT != m_type) && (Type::ARRAY != m_type)) {
        throw std::domain_error("Invalid JSON type");
    }
}

void Value::assert_type(Type type) const {
    if (type != m_type) {
        throw std::domain_error("Invalid JSON type");
    }
}

void Value::create_container(Type type) {
    m_type = type;
    switch(type) {
    case Type::OBJECT:
        new (&m_object) Object();
        break;
    case Type::ARRAY:
        new (&m_array) Array();
        break;
    case Type::STRING:
        new (&m_string) std::string();
        break;
    case Type::NUMBER:
        new (&m_number) Number();
        break;
    case Type::BOOLEAN:
        m_boolean = false;
        break;
    case Type::EMPTY:
    default:
        break;
    }
}

Value& Value::operator=(const Value& value) {
    if (&value == this) {
        return *this;
    }

    if (value.m_type != m_type) {
        this->~Value();
        create_container(value.m_type);
    }

    switch(m_type) {
    case Type::OBJECT:
        m_object = value.m_object;
        break;
    case Type::ARRAY:
        m_array = value.m_array;
        break;
    case Type::STRING:
        m_string = value.m_string;
        break;
    case Type::NUMBER:
        m_number = value.m_number;
        break;
    case Type::BOOLEAN:
        m_boolean = value.m_boolean;
        break;
    case Type::EMPTY:
    default:
        break;
    }

    return *this;
}

size_t Value::size() const {
    size_t size = 0;

    switch(m_type) {
    case Type::OBJECT:
        size = m_object.size();
        break;
    case Type::ARRAY:
        size = m_array.size();
        break;
    case Type::STRING:
        size = m_string.size();
        break;
    case Type::NUMBER:
        size = 1;
        break;
    case Type::BOOLEAN:
        size = 1;
        break;
    case Type::EMPTY:
    default:
        size = 0;
        break;
    }

    return size;
}

void Value::clear() {
    switch(m_type) {
    case Type::OBJECT:
        m_object.clear();
        break;
    case Type::ARRAY:
        m_array.clear();
        break;
    case Type::STRING:
        m_string.clear();
        break;
    case Type::NUMBER:
        m_number = 0;
        break;
    case Type::BOOLEAN:
        m_boolean = false;
        break;
    case Type::EMPTY:
    default:
        break;
    }
}

Value& Value::Value::operator[](const std::string& key) {
    assert_type(Type::OBJECT);

    for (auto& key_value : m_object) {
        if (key == key_value.first) {
            return key_value.second;
        }
    }

    m_object.emplace_back(key, std::move(Value()));

    return m_object.back().second;
}

const Value& Value::Value::operator[](const std::string& key) const {
    assert_type(Type::OBJECT);

    for (const auto& key_value : m_object) {
        if (key == key_value.first) {
            return key_value.second;
        }
    }

    throw std::out_of_range("Not found");
}

Value& Value::Value::operator[](size_t index) {
    assert_container();

    Value* value;

    if (Type::ARRAY == m_type) {
        if (m_array.size() == index) {
            m_array.emplace_back(std::move(Value()));
            value = &m_array.back();
        } else {
            value = &m_array[index];
        }
    } else {
        value = &m_object[index].second;
    }

    return *value;
}

const Value& Value::Value::operator[](size_t index) const {
    assert_container();

    const Value* value;

    if (Type::ARRAY == m_type) {
        value = &m_array[index];
    } else {
        value = &m_object[index].second;
    }

    return *value;
}

void Value::push_back(const Value& value) {
    assert_type(Type::ARRAY);
    m_array.push_back(value);
}

bool Value::operator==(const Value& value) const {
    if (value.m_type != m_type) {
        return false;
    }

    bool result = false;

    switch(m_type) {
    case Type::OBJECT:
        break;
    case Type::ARRAY:
        break;
    case Type::STRING:
        result = (value == m_string);
        break;
    case Type::NUMBER:
        result = (value.m_number == m_number.type());
        break;
    case Type::BOOLEAN:
        result = (value == m_boolean);
        break;
    case Type::EMPTY:
        result = (value == nullptr);
        break;
    default:
        result = false;
        break;
    }

    return result;
}

Value::operator const std::string&() const {
    assert_type(Type::STRING);
    return m_string;
}

Value::operator const char*() const {
    assert_type(Type::STRING);
    return m_string.c_str();
}

Value::operator bool() const {
    assert_type(Type::BOOLEAN);
    return m_boolean;
}

Value::operator std::nullptr_t() const {
    assert_type(Type::EMPTY);
    return nullptr;
}

Value::operator int32_t() const {
    assert_type(Type::NUMBER);
    return int32_t(m_number);
}

Value::operator uint32_t() const {
    assert_type(Type::NUMBER);
    return uint32_t(m_number);
}

Value::operator uint64_t() const {
    assert_type(Type::NUMBER);
    return uint64_t(m_number);
}

Value::operator double() const {
    assert_type(Type::NUMBER);
    return double(m_number);
}

bool Value::operator==(const std::string& str) const {
    assert_type(Type::STRING);
    return (str == m_string);
}

bool Value::operator==(const char* str) const {
    assert_type(Type::STRING);
    return (str == m_string);
}

bool Value::operator==(bool boolean) const {
    assert_type(Type::BOOLEAN);
    return (boolean == m_boolean);
}

bool Value::operator==(std::nullptr_t empty) const {
    assert_type(Type::EMPTY);
    return (empty == nullptr);
}

bool Value::operator==(int32_t value) const {
    assert_type(Type::NUMBER);
    return int32_t(m_number) == value;
}

bool Value::operator==(int64_t value) const {
    assert_type(Type::NUMBER);
    return int64_t(m_number) == value;
}

bool Value::operator==(uint32_t value) const {
    assert_type(Type::NUMBER);
    return uint32_t(m_number) == value;
}

bool Value::operator==(uint64_t value) const {
    assert_type(Type::NUMBER);
    return uint64_t(m_number) == value;
}

bool Value::operator==(double value) const {
    assert_type(Type::NUMBER);
    return fabs(value - double(m_number)) <
        std::numeric_limits<double>::epsilon();
}
