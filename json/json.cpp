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
#include <limits>
#include <stdexcept>

using namespace json;

Value::Number::operator Uint() const {
    Uint value;

    switch (m_type) {
    case Type::INT:
        if (std::signbit(m_int)) {
            throw std::underflow_error("Number underflow");
        }
        value = Uint(m_int);
        break;
    case Type::UINT:
        value = m_uint;
        break;
    case Type::DOUBLE:
        if (std::signbit(m_Double)) {
            throw std::underflow_error("Number underflow");
        }
        value = Uint(std::round(m_Double));
        break;
    default:
        value = 0;
        break;
    }

    return value;
}

Value::Number::operator Int() const {
    Int value;

    switch (m_type) {
    case Type::INT:
        value = m_int;
        break;
    case Type::UINT:
        if (std::numeric_limits<Int>::max() < m_uint) {
            throw std::overflow_error("Number overflow");
        }
        value = Int(m_uint);
        break;
    case Type::DOUBLE:
        value = Int(std::round(m_Double));
        break;
    default:
        value = 0;
        break;
    }

    return value;
}

Value::Number::operator Double() const {
    Double value;

    switch (m_type) {
    case Type::INT:
        value = m_int;
        break;
    case Type::UINT:
        value = m_uint;
        break;
    case Type::DOUBLE:
        value = m_Double;
        break;
    default:
        value = 0.0;
        break;
    }

    return value;
}

bool Value::Number::operator==(const Number& number) const {
    bool result = false;

    switch (m_type) {
    case Type::INT:
        result = (m_int == Int(number));
        break;
    case Type::UINT:
        result = (m_uint == Uint(number));
        break;
    case Type::DOUBLE:
        result = std::fabs(m_Double - Double(number)) <
            std::numeric_limits<Double>::epsilon();
        break;
    default:
        result = false;
        break;
    }

    return result;
}

Value::Value(Type type) : m_type(type) {
    create_container(m_type);
}

Value::Value(Null) : m_type(Type::EMPTY) { }

Value::Value(Bool boolean) : m_type(Type::BOOLEAN) {
    m_boolean = boolean;
}

Value::Value(const char* str) : m_type(Type::STRING) {
    new (&m_string) String(str);
}

Value::Value(const String& str) : m_type(Type::STRING) {
    new (&m_string) String(str);
}

Value::Value(const Pair& pair) : m_type(Type::OBJECT) {
    new (&m_object) Object{pair};
}

Value::Value(const String& key, const Value& value) :
    m_type(Type::OBJECT) {
    new (&m_object) Object{std::make_pair(key, value)};
}

Value::Value(Uint value) : m_type(Type::NUMBER) {
    new (&m_number) Number(value);
}

Value::Value(Int value) : m_type(Type::NUMBER) {
    new (&m_number) Number(value);
}

Value::Value(Double value) : m_type(Type::NUMBER) {
    new (&m_number) Number(value);
}

Value::Value(const Number& number) : m_type(Type::NUMBER) {
    new (&m_number) Number(number);
}

Value::~Value() {
    switch (m_type) {
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
        throw std::domain_error("Invalid JSON type " + std::to_string(int(type)) + " " + std::to_string(int(m_type)));
    }
}

void Value::create_container(Type type) {
    m_type = type;
    switch (type) {
    case Type::OBJECT:
        new (&m_object) Object();
        break;
    case Type::ARRAY:
        new (&m_array) Array();
        break;
    case Type::STRING:
        new (&m_string) String();
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

Value::Value(const Value& value) : m_type(value.m_type) {
    create_container(m_type);
    operator=(value);
}

Value::Value(Value&& value) : m_type(value.m_type) {
    create_container(m_type);
    operator=(std::move(value));
}

Value& Value::operator=(const Value& value) {
    if (&value == this) {
        return *this;
    }

    if (value.m_type != m_type) {
        this->~Value();
        create_container(value.m_type);
    }

    switch (m_type) {
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

Value& Value::operator=(Value&& value) {
    if (&value == this) {
        return *this;
    }

    if (value.m_type != m_type) {
        this->~Value();
        create_container(value.m_type);
    }

    switch (m_type) {
    case Type::OBJECT:
        m_object = std::move(value.m_object);
        break;
    case Type::ARRAY:
        m_array = std::move(value.m_array);
        break;
    case Type::STRING:
        m_string = std::move(value.m_string);
        break;
    case Type::NUMBER:
        m_number = std::move(value.m_number);
        break;
    case Type::BOOLEAN:
        m_boolean = std::move(value.m_boolean);
        break;
    case Type::EMPTY:
    default:
        break;
    }

    value.~Value();

    return *this;
}

size_t Value::size() const {
    size_t size = 0;

    switch (m_type) {
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
    switch (m_type) {
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
        m_number = Int(0);
        break;
    case Type::BOOLEAN:
        m_boolean = false;
        break;
    case Type::EMPTY:
    default:
        break;
    }
}

Value& Value::Value::operator[](const String& key) {
    if (Type::EMPTY == m_type) {
        operator=(std::move(Value(Type::OBJECT)));
    }
    assert_type(Type::OBJECT);

    for (auto& pair : m_object) {
        if (key == pair.first) {
            return pair.second;
        }
    }

    m_object.emplace_back(key, std::move(Value()));

    return m_object.back().second;
}

const Value& Value::Value::operator[](const String& key) const {
    assert_type(Type::OBJECT);

    for (const auto& pair : m_object) {
        if (key == pair.first) {
            return pair.second;
        }
    }

    throw std::out_of_range("Not found");
}

Value& Value::Value::operator[](size_t index) {
    if (Type::EMPTY == m_type) {
        operator=(std::move(Value(Type::ARRAY)));
    }
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

Value& Value::append(const Value& value) {
    if (Type::EMPTY == m_type) {
        operator=(std::move(Value(Type::ARRAY)));
    }
    assert_type(Type::ARRAY);
    m_array.push_back(value);
    return m_array.back();
}

bool Value::empty() const {
    return (0 == size()) ? true : false;
}

bool Value::operator==(const Value& value) const {
    if (value.m_type != m_type) {
        return false;
    }

    bool result = false;

    switch (m_type) {
    case Type::OBJECT:
        result = (m_object == value.m_object);
        break;
    case Type::ARRAY:
        result = (m_array == value.m_array);
        break;
    case Type::STRING:
        result = (m_string == value.m_string);
        break;
    case Type::NUMBER:
        result = (m_number == value.m_number);
        break;
    case Type::BOOLEAN:
        result = (m_boolean == value.m_boolean);
        break;
    case Type::EMPTY:
        result = true;
        break;
    default:
        result = false;
        break;
    }

    return result;
}

Value::operator const String&() const {
    assert_type(Type::STRING);
    return m_string;
}

Value::operator const char*() const {
    assert_type(Type::STRING);
    return m_string.c_str();
}

Value::operator Bool() const {
    assert_type(Type::BOOLEAN);
    return m_boolean;
}

Value::operator Null() const {
    assert_type(Type::EMPTY);
    return nullptr;
}

Value::operator Int() const {
    assert_type(Type::NUMBER);
    return Int(m_number);
}

Value::operator Uint() const {
    assert_type(Type::NUMBER);
    return Uint(m_number);
}

Value::operator Double() const {
    assert_type(Type::NUMBER);
    return Double(m_number);
}

Value::operator const Number&() const {
    assert_type(Type::NUMBER);
    return m_number;
}

Value::Iterator Value::begin() {
    if (Type::OBJECT == m_type) {
        return m_object.begin();
    } else if (Type::ARRAY == m_type) {
        return m_array.begin();
    }
    return Iterator();
}

Value::Iterator Value::end() {
    if (Type::OBJECT == m_type) {
        return m_object.end();
    } else if (Type::ARRAY == m_type) {
        return m_array.end();
    }
    return Iterator();
}

Value::ConstIterator Value::cbegin() const {
    if (Type::OBJECT == m_type) {
        return m_object.cbegin();
    } else if (Type::ARRAY == m_type) {
        return m_array.cbegin();
    }
    return ConstIterator();
}

Value::ConstIterator Value::cend() const {
    if (Type::OBJECT == m_type) {
        return m_object.cend();
    } else if (Type::ARRAY == m_type) {
        return m_array.cend();
    }
    return ConstIterator();
}

const char* Value::BaseIterator::key() const {
    if (Value::Type::OBJECT == m_type) {
        return m_object_const_iterator->first.c_str();
    }
    return "";
}
