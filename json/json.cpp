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

/*!
 * @brief Raw aligned template memory
 * */
template<typename T>
using raw_memory = typename std::aligned_storage<sizeof(T),
      std::alignment_of<T>::value>::type;

/*! Create raw aligned memory for Value object and initialize with zeros */
static const raw_memory<Value> g_null_value_raw {};

/*! Omit dereferencing type-punned pointer */
static const void* g_null_value_ref = &g_null_value_raw;

/*! Now we can cast raw memory to JSON value object */
static const Value& g_null_value = *static_cast<const Value*>(g_null_value_ref);

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

Number::Type Number::get_type() const {
    return m_type;
}

bool Number::is_int() const {
    return Type::INT == m_type;
}

bool Number::is_uint() const {
    return Type::UINT == m_type;
}

bool Number::is_double() const {
    return Type::DOUBLE == m_type;
}

bool json::operator==(const Number& num1, const Number& num2) {
    bool result = false;

    switch (num1.get_type()) {
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

Value::Value(Type type) : m_type(type) {
    create_container(type);
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

Value::Value(const char* key, const Value& value) : m_type(Type::OBJECT) {
    new (&m_object) Object{std::make_pair(key, value)};
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

void Value::assign(std::initializer_list<Pair> init_list) {
    *this = init_list;
}

void Value::assign(std::initializer_list<Value> init_list) {
    *this = init_list;
}

void Value::assign(size_t count, const Value& value) {
    this->~Value();
    create_container(Type::ARRAY);
    new (&m_array) Array(count, value);
}

size_t Value::size() const {
    size_t value = 0;

    switch (m_type) {
    case Type::OBJECT:
        value = m_object.size();
        break;
    case Type::ARRAY:
        value = m_array.size();
        break;
    case Type::STRING:
        value = m_string.size();
        break;
    case Type::NIL:
    case Type::NUMBER:
    case Type::BOOLEAN:
    default:
        value = 0;
        break;
    }

    return value;
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
        m_number = 0;
        break;
    case Type::BOOLEAN:
        m_boolean = false;
        break;
    case Type::NIL:
    default:
        break;
    }
}

Value& Value::operator[](int index) {
    return (*this)[size_t(index)];
}

const Value& Value::operator[](int index) const {
    return (*this)[size_t(index)];
}

Value& Value::operator[](const String& key) {
    return (*this)[key.c_str()];
}

const Value& Value::operator[](const String& key) const {
    return (*this)[key.c_str()];
}

Value::Type Value::get_type() const {
    return m_type;
}

bool Value::is_member(const std::string& key) const {
    return is_member(key.c_str());
}

bool Value::is_member(const char* key) const {
    if (!is_object()) { return false; }
    return (std::cref((*this)[key]) != g_null_value);
}


bool Value::empty() const {
    return !size();
}

size_t Value::erase(const char* key) {
    if (!is_object()) { return 0; }

    for (auto it = m_object.begin(); it != m_object.end(); it++) {
        if (key == it->first) {
            m_object.erase(it);
            return 1;
        }
    }

    return 0;
}


size_t Value::erase(const String& key) {
    return erase(key.c_str());
}

bool Value::is_string() const {
    return Type::STRING == m_type;
}

bool Value::is_object() const {
    return Type::OBJECT == m_type;
}

bool Value::is_array() const {
    return Type::ARRAY == m_type;
}

bool Value::is_number() const {
    return Type::NUMBER == m_type;
}

bool Value::is_boolean() const {
    return Type::BOOLEAN == m_type;
}

bool Value::is_null() const {
    return Type::NIL == m_type;
}

bool Value::is_int() const {
    return is_number() ? Number(m_number).is_int() : false;
}

bool Value::is_uint() const {
    return is_number() ? Number(m_number).is_uint() : false;
}

bool Value::is_double() const {
    return is_number() ? Number(m_number).is_double() : false;
}

Value::operator String&() {
    return m_string;
}

Value::operator const String&() const {
    return m_string;
}

Value::operator const char*() const {
    return m_string.c_str();
}

Value::operator Bool() const {
    return m_boolean;
}

Value::operator Null() const {
    return nullptr;
}

Value::operator Int() const {
    return Int(m_number);
}

Value::operator Uint() const {
    return Uint(m_number);
}

Value::operator Double() const {
    return Double(m_number);
}

Value::operator Array&() {
    return m_array;
}

Value::operator Number&() {
    return m_number;
}

Value::operator const Array&() const {
    return m_array;
}

Value::operator const Object&() const {
    return m_object;
}

Value::operator const Number&() const {
    return m_number;
}

bool Value::operator!() const {
    return is_null();
}

Value& Value::Value::operator[](const char* key) {
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

const Value& Value::Value::operator[](const char* key) const {
    if (!is_object()) { return *this; }

    for (const auto& pair : m_object) {
        if (key == pair.first) {
            return pair.second;
        }
    }

    return g_null_value;
}

Value& Value::Value::operator[](const size_t index) {
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

const Value& Value::Value::operator[](const size_t index) const {
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

Value::iterator Value::begin() {
    if (is_array()) { return m_array.begin(); }
    if (is_object()) { return m_object.begin(); }
    return *this;
}

Value::iterator Value::end() {
    if (is_array()) { return m_array.end(); }
    if (is_object()) { return m_object.end(); }
    return *this;
}

Value::const_iterator Value::cbegin() const {
    if (is_array()) { return m_array.cbegin(); }
    if (is_object()) { return m_object.cbegin(); }
    return *this;
}

Value::const_iterator Value::cend() const {
    if (is_array()) { return m_array.cend(); }
    if (is_object()) { return m_object.cend(); }
    return *this;
}

Value::const_iterator Value::begin() const {
    return std::move(cbegin());
}

Value::const_iterator Value::end() const {
    return std::move(cend());
}

Value::iterator::iterator() :
    m_type(Type::ARRAY)
{
    new (&m_array_iterator) Array::iterator();
}

Value::iterator::iterator(Value& it) :
    m_type(it.m_type),
    m_value_iterator(&it)
{ }

Value::iterator::iterator(const Array::iterator& it) :
    m_type(Type::ARRAY),
    m_array_iterator(it)
{
    new (&m_array_iterator) Array::iterator(it);
}

Value::iterator::iterator(const Object::iterator& it) :
    m_type(Type::OBJECT),
    m_object_iterator(it)
{
    new (&m_object_iterator) Object::iterator(it);
}

Value::iterator& Value::iterator::operator++() {
    if (Type::ARRAY == m_type) { m_array_iterator++; }
    else if (Type::OBJECT == m_type) { m_object_iterator++; }
    else { m_value_iterator++; }
    return *this;
}

Value::iterator Value::iterator::operator++(int) {
    iterator temp(*this);
    operator++();
    return temp;
}

Value::iterator::pointer Value::iterator::operator->() {
    pointer ptr;
    if (Type::ARRAY == m_type) { ptr = &(*m_array_iterator); }
    else if (Type::OBJECT == m_type) { ptr = &m_object_iterator->second; }
    else { ptr = m_value_iterator; }
    return ptr;
}

Value::iterator::reference Value::iterator::operator*() {
    return *operator->();
}

bool json::operator==(const Value::iterator& it1, const Value::iterator& it2) {
    if (it1.m_type != it2.m_type) { return false; }

    bool result;

    if (Value::Type::ARRAY == it1.m_type) {
        result = (it1.m_array_iterator == it2.m_array_iterator);
    }
    else if (Value::Type::OBJECT == it1.m_type) {
        result = (it1.m_object_iterator == it2.m_object_iterator);
    }
    else {
        result = (it1.m_value_iterator == it2.m_value_iterator);
    }

    return result;
}

bool json::operator!=(const Value::iterator& it1, const Value::iterator& it2) {
    return !(it1 == it2);
}

Value::const_iterator::const_iterator() :
    m_type(Type::ARRAY),
    m_array_const_iterator()
{
    new (&m_array_const_iterator) Array::const_iterator();
}

Value::const_iterator::const_iterator(const Value& it) :
    m_type(it.m_type),
    m_value_const_iterator(&it)
{ }

Value::const_iterator::const_iterator(const Array::const_iterator& it) :
    m_type(Type::ARRAY),
    m_array_const_iterator(it)
{
    new (&m_array_const_iterator) Array::const_iterator(it);
}

Value::const_iterator::const_iterator(const Object::const_iterator& it) :
    m_type(Type::OBJECT),
    m_object_const_iterator(it)
{
    new (&m_object_const_iterator) Object::const_iterator(it);
}

const Value::const_iterator& Value::const_iterator::operator++() {
    if (Type::ARRAY == m_type) { m_array_const_iterator++; }
    else if (Type::OBJECT == m_type) { m_object_const_iterator++; }
    else { m_value_const_iterator++; }
    return *this;
}

const Value::const_iterator Value::const_iterator::operator++(int) {
    const_iterator temp(*this);
    operator++();
    return temp;
}

Value::const_iterator::pointer Value::const_iterator::operator->() const {
    pointer ptr;
    if (Type::ARRAY == m_type) { ptr = &(*m_array_const_iterator); }
    else if (Type::OBJECT == m_type) { ptr = &m_object_const_iterator->second; }
    else { ptr = m_value_const_iterator; }
    return ptr;
}

Value::const_iterator::reference Value::const_iterator::operator*() const {
    return *operator->();
}

bool json::operator==(const Value::const_iterator& it1,
        const Value::const_iterator& it2) {
    if (it1.m_type != it2.m_type) { return false; }

    bool result;

    if (Value::Type::ARRAY == it1.m_type) {
        result = (it1.m_array_const_iterator == it2.m_array_const_iterator);
    }
    else if (Value::Type::OBJECT == it1.m_type) {
        result = (it1.m_object_const_iterator == it2.m_object_const_iterator);
    }
    else {
        result = (it1.m_value_const_iterator == it2.m_value_const_iterator);
    }

    return result;
}

bool json::operator!=(const Value::const_iterator& it1,
        const Value::const_iterator& it2) {
    return !(it1 == it2);
}
