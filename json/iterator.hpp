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
 * @file json/iterator.hpp
 *
 * @brief JSON iterator interface
 * */

#ifndef _JSON_ITERATOR_HPP_
#define _JSON_ITERATOR_HPP_

#include "value.hpp"

#include <utility>
#include <iterator>

namespace json {

template<bool is_const>
class base_iterator;

template<bool is_const = false>
base_iterator<is_const> operator+(const base_iterator<is_const>&,
       typename  base_iterator<is_const>::difference_type);

template<bool is_const = false>
base_iterator<is_const> operator+(
        typename base_iterator<is_const>::difference_type,
        const base_iterator<is_const>&);

template<bool is_const = false>
base_iterator<is_const> operator-(const base_iterator<is_const>&,
        typename base_iterator<is_const>::difference_type);

template<bool is_const = false>
class base_iterator {
public:
    template<bool B, class T, class F>
    using conditional_t = typename std::conditional<B, T, F>::type;

    using value_type = Value;

    using difference_type = std::ptrdiff_t;

    using pointer = conditional_t<is_const, const value_type*, value_type*>;

    using reference = conditional_t<is_const, const value_type&, value_type&>;

    using iterator_category = std::random_access_iterator_tag;

    using value_iterator = pointer;

    using array_iterator = conditional_t<is_const,
          Array::const_iterator, Array::iterator>;

    using object_iterator = conditional_t<is_const,
          Object::const_iterator, Object::iterator>;

    base_iterator();

    base_iterator(const base_iterator& it) = default;

    base_iterator(base_iterator&& it) = default;

    template<typename = typename std::enable_if<is_const>>
    base_iterator(const base_iterator<>& it);

    base_iterator(const value_iterator& it);

    base_iterator(const array_iterator& it);

    base_iterator(const object_iterator& it);

    const char* key() const;

    bool is_array() const;

    bool is_object() const;

    template<typename = typename std::enable_if<is_const>>
    base_iterator& operator=(const base_iterator<>& it);

    base_iterator& operator=(const base_iterator& it) = default;

    base_iterator& operator=(base_iterator&& it) = default;

    base_iterator& operator++();

    base_iterator operator++(int);

    reference operator*() const;

    pointer operator->() const;

    reference operator[](difference_type) const;

    base_iterator& operator+=(difference_type);
    base_iterator& operator-=(difference_type);

    friend Value;

    friend base_iterator<true>;

    friend bool operator<(const base_iterator<true>&,
            const base_iterator<true>&);

    friend bool operator>(const base_iterator<true>&,
            const base_iterator<true>&);

    friend bool operator<=(const base_iterator<true>&,
            const base_iterator<true>&);

    friend bool operator>=(const base_iterator<true>&,
            const base_iterator<true>&);

    friend base_iterator operator+<>(const base_iterator&,
            base_iterator::difference_type);

    friend base_iterator operator+<>(base_iterator::difference_type,
            const base_iterator&);

    friend base_iterator operator-<>(const base_iterator&,
            base_iterator::difference_type);

    friend base_iterator::difference_type operator-(
            base_iterator<true>,
            base_iterator<true>);

    friend bool operator==(
            const base_iterator<true>&,
            const base_iterator<true>&);

    friend bool operator!=(
            const base_iterator<true>&,
            const base_iterator<true>&);

    friend void swap(base_iterator<>&, base_iterator<>&);
private:
    typename Value::Type m_type;

    union {
        value_iterator m_value_iterator;
        array_iterator m_array_iterator;
        object_iterator m_object_iterator;
    };
};

bool operator<(const base_iterator<true>&, const base_iterator<true>&);

bool operator==(const base_iterator<true>&, const base_iterator<true>&);

bool operator!=(const base_iterator<true>&, const base_iterator<true>&);

bool operator<(const base_iterator<true>&, const base_iterator<true>&);

bool operator>(const base_iterator<true>&, const base_iterator<true>&);

bool operator<=(const base_iterator<true>&, const base_iterator<true>&);

bool operator>=(const base_iterator<true>&, const base_iterator<true>&);

base_iterator<>::difference_type operator-(base_iterator<true>,
        base_iterator<true>);

void swap(base_iterator<>&, base_iterator<>&);

} /* namespace json */

#endif /* _JSON_ITERATOR_HPP_ */
