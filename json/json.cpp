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

Number::Number() : m_type(Type::INT), m_int(0) { }

Number::Number(Int value) : m_type(Type::INT), m_int(value) { }

Number::Number(Uint value) : m_type(Type::UINT), m_uint(value) { }

Number::Number(Double value) : m_type(Type::DOUBLE), m_Double(value) { }

Number::operator Uint() const {
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

Number::operator Int() const {
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

Number::operator Double() const {
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

bool Number::operator==(const Number& number) const {
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

Value::Value(const Pair& pair) : m_type(Type::MEMBERS) {
    new (&m_members) Members{pair};
}

Value::Value(const String& key, const Value& value) : m_type(Type::MEMBERS) {
    new (&m_members) Members{std::make_pair(key, value)};
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

Value::Value(size_t count, const Value& value) : m_type(Type::ARRAY) {
    new (&m_array) Array(count, value);
}

Value::Value(std::initializer_list<Pair> init_list) : m_type(Type::MEMBERS) {
    new (&m_members) Members();

    for (const auto& pair : init_list) {
        (*this)[pair.first] = pair.second;
    }
}

Value::Value(std::initializer_list<Value> init_list) : m_type(Type::ARRAY) {
    new (&m_array) Array();
    m_array = init_list;
}

Value::~Value() {
    switch (m_type) {
    case Type::MEMBERS:
        m_members.~vector();
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

void Value::assert_container(Type new_type) {
    if ((Type::MEMBERS == m_type) || (Type::ARRAY == m_type)) return;

    if (Type::EMPTY == m_type) {
        *this = std::move(Value(new_type));
    } else {
        throw std::domain_error("Invalid JSON type ");
    }
}

void Value::assert_container() const {
    if ((Type::MEMBERS == m_type) || (Type::ARRAY == m_type)) return;

    throw std::domain_error("Invalid JSON type");
}

void Value::assert_type(Type type) {
    if (m_type == type) return;

    if (Type::EMPTY == m_type) {
        *this = std::move(Value(type));
    } else {
        throw std::domain_error("Invalid JSON type ");
    }
}

void Value::assert_type(Type type) const {
    if (m_type == type) return;

    throw std::domain_error("Invalid JSON type ");
}

void Value::create_container(Type type) {
    m_type = type;
    switch (type) {
    case Type::MEMBERS:
        new (&m_members) Members();
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
    case Type::MEMBERS:
        m_members = value.m_members;
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
    case Type::MEMBERS:
        m_members = std::move(value.m_members);
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

    value.m_type = Type::EMPTY;

    return *this;
}

Value& Value::operator=(std::initializer_list<Pair> init_list) {
    this->~Value();
    create_container(Type::MEMBERS);

    for (const auto& pair : init_list) {
        (*this)[pair.first] = pair.second;
    }

    return *this;
}

Value& Value::operator=(std::initializer_list<Value> init_list) {
    this->~Value();
    create_container(Type::ARRAY);

    m_array = init_list;

    return *this;
}

void Value::assign(size_t count, const Value& value) {
    this->~Value();
    create_container(Type::ARRAY);
    new (&m_array) Array(count, value);
}

void Value::assign(std::initializer_list<Pair> init_list) {
    operator=(init_list);
}

void Value::assign(std::initializer_list<Value> init_list) {
    operator=(init_list);
}

size_t Value::size() const {
    size_t size = 0;

    switch (m_type) {
    case Type::MEMBERS:
        size = m_members.size();
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

bool Value::empty() const {
    return !size();
}

void Value::clear() {
    switch (m_type) {
    case Type::MEMBERS:
        m_members.clear();
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

size_t Value::erase(const String& key) {
    assert_type(Type::MEMBERS);

    for (auto it = m_members.begin(); it != m_members.end(); it++) {
        if (key == it->first) {
            m_members.erase(it);
            return 1;
        }
    }

    return 0;
}

Value& Value::Value::operator[](const String& key) {
    assert_type(Type::MEMBERS);

    for (auto& pair : m_members) {
        if (key == pair.first) {
            return pair.second;
        }
    }

    m_members.emplace_back(key, std::move(Value()));

    return m_members.back().second;
}

const Value& Value::Value::operator[](const String& key) const {
    assert_type(Type::MEMBERS);

    for (const auto& pair : m_members) {
        if (key == pair.first) {
            return pair.second;
        }
    }

    throw std::out_of_range("Not found");
}

Value& Value::Value::operator[](size_t index) {
    assert_container(Type::ARRAY);

    if (Type::ARRAY == m_type) {
        if (size() == index) {
            m_array.emplace_back(std::move(Value()));
        }
        return m_array[index];
    }
    return m_members[index].second;
}

const Value& Value::Value::operator[](size_t index) const {
    assert_container();

    if (Type::ARRAY == m_type) {
        return m_array[index];
    }
    return m_members[index].second;
}

void Value::push_back(const Value& value) {
    assert_type(Type::ARRAY);

    m_array.push_back(value);
}

void Value::push_back(const Pair& pair) {
    assert_container(Type::MEMBERS);

    if (Type::MEMBERS == m_type) {
        operator[](pair.first) = pair.second;
    } else {
        m_array.push_back(pair);
    }
}

void Value::pop_back() {
    if (Type::MEMBERS == m_type) {
        m_members.pop_back();
    } else if (Type::ARRAY == m_type) {
        m_array.pop_back();
    } else {
        *this = std::move(Value());
    }
}

void Value::swap(Value& value) {
    Value temp(std::move(value));
    value = std::move(*this);
    *this = std::move(temp);
}

bool json::operator==(const Value& val1, const Value& val2) {
    if (val1.m_type != val2.m_type) {
        return false;
    }

    bool result = false;

    switch (val1.m_type) {
    case Value::Type::MEMBERS:
        result = (val1.m_members == val2.m_members);
        break;
    case Value::Type::ARRAY:
        result = (val1.m_array == val2.m_array);
        break;
    case Value::Type::STRING:
        result = (val1.m_string == val2.m_string);
        break;
    case Value::Type::NUMBER:
        result = (val1.m_number == val2.m_number);
        break;
    case Value::Type::BOOLEAN:
        result = (val1.m_boolean == val2.m_boolean);
        break;
    case Value::Type::EMPTY:
        result = true;
        break;
    default:
        result = false;
        break;
    }

    return result;
}

bool json::operator!=(const Value& val1, const Value& val2) {
    return !operator==(val1, val2);
}

Value::operator String&() {
    assert_type(Type::STRING);
    return m_string;
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

Value::operator Array&() {
    assert_type(Type::ARRAY);
    return m_array;
}

Value::operator Number&() {
    assert_type(Type::NUMBER);
    return m_number;
}

Value::operator const Array&() const {
    assert_type(Type::ARRAY);
    return m_array;
}

Value::operator const Members&() const {
    assert_type(Type::MEMBERS);
    return m_members;
}

Value::operator const Number&() const {
    assert_type(Type::NUMBER);
    return m_number;
}

Value::Iterator Value::begin() {
    if (Type::MEMBERS == m_type) {
        return m_members.begin();
    } else if (Type::ARRAY == m_type) {
        return m_array.begin();
    }
    return Iterator();
}

Value::Iterator Value::end() {
    if (Type::MEMBERS == m_type) {
        return m_members.end();
    } else if (Type::ARRAY == m_type) {
        return m_array.end();
    }
    return Iterator();
}

Value::ConstIterator Value::begin() const {
    return std::move(cbegin());
}

Value::ConstIterator Value::end() const {
    return std::move(cend());
}

Value::ConstIterator Value::cbegin() const {
    if (Type::MEMBERS == m_type) {
        return m_members.cbegin();
    } else if (Type::ARRAY == m_type) {
        return m_array.cbegin();
    }
    return ConstIterator();
}

Value::ConstIterator Value::cend() const {
    if (Type::MEMBERS == m_type) {
        return m_members.cend();
    } else if (Type::ARRAY == m_type) {
        return m_array.cend();
    }
    return ConstIterator();
}

Value::BaseIterator::BaseIterator(const Array::iterator& it) :
    m_type(Value::Type::ARRAY),
    m_array_iterator(it) { }

Value::BaseIterator::BaseIterator(const Array::const_iterator& it) :
    m_type(Value::Type::ARRAY),
    m_array_const_iterator(it) { }

Value::BaseIterator::BaseIterator(const Members::iterator& it) :
    m_type(Value::Type::MEMBERS),
    m_members_iterator(it) { }

Value::BaseIterator::BaseIterator(const Members::const_iterator& it) :
    m_type(Value::Type::MEMBERS),
    m_members_const_iterator(it) { }

bool json::operator!=(const Value::BaseIterator& it1,
        const Value::BaseIterator& it2) { return !operator==(it1, it2); }

bool json::operator==(const Value::BaseIterator& it1,
        const Value::BaseIterator& it2) {
    if (it1.m_type != it2.m_type) {
        return false;
    }

    if (Value::Type::MEMBERS == it1.m_type) {
        return (it1.m_members_iterator == it2.m_members_iterator);
    }
    return (it1.m_array_iterator == it2.m_array_iterator);
}

void Value::BaseIterator::increment() {
    if (Value::Type::MEMBERS == m_type) {
        m_members_iterator++;
    } else {
        m_array_iterator++;
    }
}

void Value::BaseIterator::decrement() {
    if (Value::Type::MEMBERS == m_type) {
        m_members_iterator--;
    } else {
        m_array_iterator--;
    }
}

void Value::BaseIterator::const_increment() {
    if (Value::Type::MEMBERS == m_type) {
        m_members_const_iterator++;
    } else {
        m_array_const_iterator++;
    }
}

void Value::BaseIterator::const_decrement() {
    if (Value::Type::MEMBERS == m_type) {
        m_members_const_iterator--;
    } else {
        m_array_const_iterator--;
    }
}

Value& Value::BaseIterator::deref() {
    if (Value::Type::MEMBERS == m_type) {
        return m_members_iterator->second;
    }
    return *m_array_iterator;
}

const Value& Value::BaseIterator::const_deref() const {
    if (Value::Type::MEMBERS == m_type) {
        return m_members_const_iterator->second;
    }
    return *m_array_const_iterator;
}

Value::Iterator::Iterator() : BaseIterator(Array::iterator{}) { }

Value::Iterator::Iterator(const Array::iterator& it) : BaseIterator(it) { }

Value::Iterator::Iterator(const Members::iterator& it) : BaseIterator(it) { }

Value::Iterator& Value::Iterator::operator++() {
    increment();
    return *this;
}

Value::Iterator Value::Iterator::operator++(int) {
    Iterator temp(*this);
    increment();
    return temp;
}

Value::ConstIterator::ConstIterator() : BaseIterator(Array::iterator{}) { }

Value::ConstIterator::ConstIterator(const Array::const_iterator& it) :
    BaseIterator(it) { }

Value::ConstIterator::ConstIterator(const Members::const_iterator& it) :
    BaseIterator(it) { }

const Value::ConstIterator& Value::ConstIterator::operator++() {
    const_increment();
    return *this;
}

const Value::ConstIterator Value::ConstIterator::operator++(int) {
    ConstIterator temp(*this);
    const_increment();
    return temp;
}
