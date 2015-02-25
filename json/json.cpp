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
#include <type_traits>
#include <functional>

using namespace json;

using aligned_value = std::aligned_storage<sizeof(Value),
      std::alignment_of<Value>::value>::type;

static const aligned_value _aligned_value{};
static const Value& g_null_value = reinterpret_cast<const Value&>(_aligned_value);

Number::Number() : m_type(Type::INT), m_int(0) { }

Number::Number(Int value) : m_type(Type::INT), m_int(value) { }

Number::Number(Uint value) : m_type(Type::UINT), m_uint(value) { }

Number::Number(Double value) : m_type(Type::DOUBLE), m_double(value) { }

Number::operator Uint() const {
    Uint value;

    switch (m_type) {
    case Type::INT:
        value = std::signbit(m_int) ? 0 : Uint(m_int);
        break;
    case Type::UINT:
        value = m_uint;
        break;
    case Type::DOUBLE:
        value = std::signbit(m_double) ? 0 : Uint(std::round(m_double));
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
        value = (std::numeric_limits<Int>::max() < m_uint) ?
            std::numeric_limits<Int>::max() : Int(m_uint);
        break;
    case Type::DOUBLE:
        value = Int(std::round(m_double));
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
        value = m_double;
        break;
    default:
        value = 0.0;
        break;
    }

    return value;
}

bool json::operator==(const Number& num1, const Number& num2) {
    bool result = false;

    switch (num1.type()) {
    case Number::Type::INT:
        result = (num1.m_int == Int(num2));
        break;
    case Number::Type::UINT:
        result = (num1.m_uint == Uint(num2));
        break;
    case Number::Type::DOUBLE:
        result = std::fabs(num1.m_double - Double(num2)) <
            std::numeric_limits<Double>::epsilon();
        break;
    default:
        result = false;
        break;
    }

    return result;
}

bool json::operator!=(const Number& num1, const Number& num2) {
    return !(num1 == num2);
}

Value::Value(Type type) : m_type(type) {
    create_container(m_type);
}

Value::Value(Null) : m_type(Type::NIL) { }

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

Value::Value(const String& key, const Value& value) : m_type(Type::OBJECT) {
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

Value::Value(size_t count, const Value& value) : m_type(Type::ARRAY) {
    new (&m_array) Array(count, value);
}

Value::Value(std::initializer_list<Pair> init_list) : m_type(Type::OBJECT) {
    new (&m_object) Object();

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
    case Type::NIL:
    default:
        break;
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
    case Type::NIL:
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
    case Type::NIL:
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
    case Type::NIL:
    default:
        break;
    }

    value.m_type = Type::NIL;

    return *this;
}

Value& Value::operator=(std::initializer_list<Pair> init_list) {
    this->~Value();
    create_container(Type::OBJECT);

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
    case Type::NIL:
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
    case Type::NIL:
    default:
        break;
    }
}

bool Value::is_member(const char* key) const {
    if (!is_object()) { return false; }
    return (std::cref((*this)[key]) != g_null_value);
}

bool Value::is_member(const std::string& key) const {
    return is_member(key.c_str());
}

size_t Value::erase(const String& key) {
    if (!is_object()) { return 0; }

    for (auto it = m_object.begin(); it != m_object.end(); it++) {
        if (key == it->first) {
            m_object.erase(it);
            return 1;
        }
    }

    return 0;
}

Value& Value::Value::operator[](const String& key) {
    if (!is_object()) {
        if (is_null()) { *this = Type::OBJECT; }
        else { return *this; }
    }

    for (auto& pair : m_object) {
        if (key == pair.first) {
            return pair.second;
        }
    }

    m_object.emplace_back(key, std::move(Value()));

    return m_object.back().second;
}

const Value& Value::Value::operator[](const String& key) const {
    if (!is_object()) { return *this; }

    for (const auto& pair : m_object) {
        if (key == pair.first) {
            return pair.second;
        }
    }

    return g_null_value;
}

Value& Value::Value::operator[](size_t index) {
    if (is_null()) { *this = Type::ARRAY; }

    if (is_array()) {
        if (size() == index) {
            m_array.emplace_back(std::move(Value()));
        }
        return m_array[index];
    }

    if  (is_object()) {
        return m_object[index].second;
    }

    return *this;
}

const Value& Value::Value::operator[](size_t index) const {
    if (is_array()) {
        return m_array[index];
    }

    if (is_object()) {
        return m_object[index].second;
    }

    return *this;
}

void Value::push_back(const Value& value) {
    if (is_null()) { *this = Value(Type::ARRAY); }

    if (is_array()) {
        m_array.push_back(value);
    }
}

void Value::push_back(const Pair& pair) {
    if (is_null()) { *this = Type::OBJECT; }

    if (is_object()) {
        (*this)[pair.first] = pair.second;
        return;
    }

    if (is_array()) {
        m_array.push_back(pair);
        return;
    }
}

void Value::pop_back() {
    if (is_array()) {
        m_array.pop_back();
        return;
    }

    if (is_object()) {
        m_object.pop_back();
        return;
    }

    *this = Type::NIL;
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
    case Value::Type::OBJECT:
        result = (val1.m_object == val2.m_object);
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
    case Value::Type::NIL:
        result = true;
        break;
    default:
        result = false;
        break;
    }

    return result;
}

bool json::operator!=(const Value& val1, const Value& val2) {
    return !(val1 == val2);
}

Value::Iterator Value::begin() {
    if (is_object()) {
        return m_object.begin();
    }

    if (is_array()) {
        return m_array.begin();
    }

    return Iterator();
}

Value::Iterator Value::end() {
    if (is_object()) {
        return m_object.end();
    }

    if (is_array()) {
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
    if (is_object()) {
        return m_object.cbegin();
    }

    if (is_array()) {
        return m_array.cbegin();
    }

    return ConstIterator();
}

Value::ConstIterator Value::cend() const {
    if (is_object()) {
        return m_object.cend();
    }

    if (is_array()) {
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

Value::BaseIterator::BaseIterator(const Object::iterator& it) :
    m_type(Value::Type::OBJECT),
    m_object_iterator(it) { }

Value::BaseIterator::BaseIterator(const Object::const_iterator& it) :
    m_type(Value::Type::OBJECT),
    m_object_const_iterator(it) { }

bool json::operator==(const Value::BaseIterator& it1,
        const Value::BaseIterator& it2) {
    if (it1.m_type != it2.m_type) {
        return false;
    }

    if (it1.is_object()) {
        return (it1.m_object_iterator == it2.m_object_iterator);
    }

    if (it1.is_array()) {
        return (it1.m_array_iterator == it2.m_array_iterator);
    }

    return false;
}

bool json::operator!=(const Value::BaseIterator& it1,
        const Value::BaseIterator& it2) { return !(it1 == it2); }

void Value::BaseIterator::increment() {
    if (is_object()) {
        m_object_iterator++;
    } else {
        m_array_iterator++;
    }
}

void Value::BaseIterator::decrement() {
    if (is_object()) {
        m_object_iterator--;
    } else {
        m_array_iterator--;
    }
}

void Value::BaseIterator::const_increment() {
    if (is_object()) {
        m_object_const_iterator++;
    } else {
        m_array_const_iterator++;
    }
}

void Value::BaseIterator::const_decrement() {
    if (is_object()) {
        m_object_const_iterator--;
    } else {
        m_array_const_iterator--;
    }
}

Value& Value::BaseIterator::deref() {
    if (is_object()) {
        return m_object_iterator->second;
    }
    return *m_array_iterator;
}

const Value& Value::BaseIterator::const_deref() const {
    if (is_object()) {
        return m_object_const_iterator->second;
    }
    return *m_array_const_iterator;
}

Value::Iterator::Iterator() : BaseIterator(Array::iterator{}) { }

Value::Iterator::Iterator(const Array::iterator& it) : BaseIterator(it) { }

Value::Iterator::Iterator(const Object::iterator& it) : BaseIterator(it) { }

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

Value::ConstIterator::ConstIterator(const Object::const_iterator& it) :
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
