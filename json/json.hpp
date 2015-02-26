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

#include <string>
#include <vector>
#include <utility>
#include <iterator>

namespace json {

class Value;

template<bool is_const>
class base_iterator;

using String = std::string;
using Pair = std::pair<String, Value>;
using Object = std::vector<Pair>;
using Array = std::vector<Value>;
using Bool = bool;
using Null = std::nullptr_t;
using Uint = unsigned int;
using Int = int;
using Double = double;

class Number {
public:
    enum class Type {
        INT,
        UINT,
        DOUBLE
    };

    Number();

    Number(Int value);

    Number(Uint value);

    Number(Double value);

    Number& operator+=(const Number& number);

    explicit operator Int() const;

    explicit operator Uint() const;

    explicit operator Double() const;

    bool is_int() const;

    bool is_uint() const;

    bool is_double() const;

    Type get_type() const;

    friend bool operator==(const Number& num1, const Number& num2);

    friend bool operator!=(const Number& num1, const Number& num2);
private:
    enum Type m_type;

    union {
        Int m_int;
        Uint m_uint;
        Double m_double;
    };
};

class Value {
public:
    enum class Type {
        NIL,
        OBJECT,
        ARRAY,
        STRING,
        NUMBER,
        BOOLEAN
    };

    Value(Type type = Type::NIL);

    Value(Null);

    Value(Bool Boolean);

    Value(const char* str);

    Value(const String& str);

    Value(const Pair& pair);

    Value(const char* key, const Value& value);

    Value(const String& key, const Value& value);

    Value(Uint value);

    Value(Int value);

    Value(Double value);

    Value(const Number& number);

    Value(size_t count, const Value& value);

    Value(std::initializer_list<Pair> init_list);

    Value(std::initializer_list<Value> init_list);

    Value(const Value& value);

    Value(Value&& value);

    ~Value();

    Value& operator=(const Value& value);

    Value& operator=(Value&& value);

    Value& operator=(std::initializer_list<Pair> init_list);

    Value& operator=(std::initializer_list<Value> init_list);

    Value& operator+=(const Value& value);

    void assign(size_t count, const Value& value);

    void assign(std::initializer_list<Pair> init_list);

    void assign(std::initializer_list<Value> init_list);

    void push_back(const Pair& pair);

    void push_back(const Value& value);

    void pop_back();

    size_t size() const;

    void clear();

    bool empty() const;

    size_t erase(const String& key);

    size_t erase(const char* key);

    void swap(Value& value);

    Value& operator[](size_t index);

    const Value& operator[](size_t index) const;

    Value& operator[](int index);

    const Value& operator[](int index) const;

    Value& operator[](const char* key);

    const Value& operator[](const char* key) const;

    Value& operator[](const String& key);

    const Value& operator[](const String& key) const;

    Type get_type() const;

    bool is_member(const std::string& key) const;

    bool is_member(const char* key) const;

    bool is_string() const;

    bool is_object() const;

    bool is_array() const;

    bool is_number() const;

    bool is_boolean() const;

    bool is_null() const;

    bool is_int() const;

    bool is_uint() const;

    bool is_double() const;

    explicit operator String&();

    explicit operator const String&() const;

    explicit operator const char*() const;

    explicit operator Bool() const;

    explicit operator Null() const;

    explicit operator Int() const;

    explicit operator Uint() const;

    explicit operator Double() const;

    explicit operator Array&();

    explicit operator Number&();

    explicit operator const Array&() const;

    explicit operator const Object&() const;

    explicit operator const Number&() const;

    bool operator!() const;

    friend bool operator==(const Value&, const Value&);
    friend bool operator!=(const Value&, const Value&);
    friend bool operator<(const Value&, const Value&);
    friend bool operator>(const Value&, const Value&);
    friend bool operator<=(const Value&, const Value&);
    friend bool operator>=(const Value&, const Value&);

    using iterator = base_iterator<false>;
    using const_iterator = base_iterator<true>;

    iterator begin();

    iterator end();

    const_iterator begin() const;

    const_iterator end() const;

    const_iterator cbegin() const;

    const_iterator cend() const;

private:
    enum Type m_type;

    union {
        Object m_object;
        Array m_array;
        String m_string;
        Number m_number;
        Bool m_boolean;
    };

    void create_container(Type type);
};

template<bool is_const>
bool operator<(
        const base_iterator<is_const>&,
        const base_iterator<is_const>&);

template<bool is_const>
bool operator==(
        const base_iterator<is_const>&,
        const base_iterator<is_const>&);

template<bool is_const>
bool operator!=(
        const base_iterator<is_const>&,
        const base_iterator<is_const>&);

template<bool is_const>
bool operator<(
        const base_iterator<is_const>&,
        const base_iterator<is_const>&);

template<bool is_const>
bool operator>(
        const base_iterator<is_const>&,
        const base_iterator<is_const>&);

template<bool is_const>
bool operator<=(
        const base_iterator<is_const>&,
        const base_iterator<is_const>&);

template<bool is_const>
bool operator>=(
        const base_iterator<is_const>&,
        const base_iterator<is_const>&);

template<bool is_const>
base_iterator<is_const> operator+(
        const base_iterator<is_const>&,
        typename base_iterator<is_const>::difference_type);

template<bool is_const>
base_iterator<is_const> operator+(
        typename base_iterator<is_const>::difference_type,
        const base_iterator<is_const>&);

template<bool is_const>
base_iterator<is_const> operator-(
        const base_iterator<is_const>&,
        typename base_iterator<is_const>::difference_type);

template<bool is_const>
typename base_iterator<is_const>::difference_type operator-(
        base_iterator<is_const>,
        base_iterator<is_const>);

template<bool is_const>
void swap(base_iterator<is_const>&, base_iterator<is_const>&);

template<bool is_const>
class base_iterator {
public:
    template<bool B, class T, class F>
    using conditional_t = typename std::conditional<B, T, F>::type;

    using value_type = Value;

    using difference_type = std::ptrdiff_t;

    using pointer = conditional_t<is_const, const value_type*, value_type*>;

    using reference = conditional_t<is_const, const value_type&, value_type&>;

    using iterator_category = std::random_access_iterator_tag;

    using array_iterator = conditional_t<is_const,
          Array::const_iterator, Array::iterator>;

    using object_iterator = conditional_t<is_const,
          Object::const_iterator, Object::iterator>;

    base_iterator();

    base_iterator(reference it);

    base_iterator(const array_iterator& it);

    base_iterator(const object_iterator& it);

    const char* key() const;

    base_iterator& operator++();

    base_iterator operator++(int);

    reference operator*();

    pointer operator->();

    reference operator[](difference_type);

    base_iterator& operator+=(difference_type);
    base_iterator& operator-=(difference_type);

    friend bool operator< <>(const base_iterator&, const base_iterator&);
    friend bool operator><>(const base_iterator&, const base_iterator&);
    friend bool operator<=<>(const base_iterator&, const base_iterator&);
    friend bool operator>=<>(const base_iterator&, const base_iterator&);

    friend base_iterator operator+<>(
            const base_iterator&,
            base_iterator::difference_type);

    friend base_iterator operator+<>(
            base_iterator::difference_type,
            const base_iterator&);

    friend base_iterator operator-<>(
            const base_iterator&,
            base_iterator::difference_type);

    friend base_iterator::difference_type operator-<>(
            base_iterator,
            base_iterator);

    friend bool operator==<>(
            const base_iterator&,
            const base_iterator&);

    friend bool operator!=<>(
            const base_iterator&,
            const base_iterator&);

    friend void swap<>(base_iterator&, base_iterator&);
private:
    typename Value::Type m_type;

    union {
        pointer m_value_iterator;
        array_iterator m_array_iterator;
        object_iterator m_object_iterator;
    };
};

bool operator==(const Value&, const Value&);
bool operator!=(const Value&, const Value&);
bool operator<(const Value&, const Value&);
bool operator>(const Value&, const Value&);
bool operator<=(const Value&, const Value&);
bool operator>=(const Value&, const Value&);

} /* namespace json */

#endif /* _JSON_HPP_ */
