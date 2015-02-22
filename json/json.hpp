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

class Value {
public:
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

        Number() : m_type(Type::INT), m_int(0) { }

        Number(Int value) : m_type(Type::INT), m_int(value) { }

        Number(Uint value) : m_type(Type::UINT), m_uint(value) { }

        Number(Double value) : m_type(Type::DOUBLE), m_Double(value) { }

        operator Int() const;

        operator Uint() const;

        operator Double() const;

        bool operator==(const Number& number) const;

        Type type() const { return m_type; }
    private:
        enum Type m_type;

        union {
            Int m_int;
            Uint m_uint;
            Double m_Double;
        };
    };

    enum class Type {
        OBJECT,
        ARRAY,
        STRING,
        NUMBER,
        BOOLEAN,
        EMPTY
    };

    Value(Type type = Type::EMPTY);

    Value(Null);

    Value(Bool Boolean);

    Value(const char* str);

    Value(const String& str);

    Value(const Pair& pair);

    Value(const String& key, const Value& value);

    Value(Uint value);

    Value(Int value);

    Value(Double value);

    Value(const Number& number);

    Value(const Value& value);

    Value(Value&& value);

    ~Value();

    Value& operator=(const Value& value);

    Value& operator=(Value&& value);

    Value& append(const Value& value);

    size_t size() const;

    void clear();

    bool empty() const;

    Value& operator[](const String& key);

    const Value& operator[](const String& key) const;

    Value& operator[](size_t index);

    const Value& operator[](size_t index) const;

    Type type() const { return m_type; }

    explicit operator const String&() const;

    explicit operator const char*() const;

    explicit operator Bool() const;

    explicit operator Null() const;

    explicit operator Int() const;

    explicit operator Uint() const;

    explicit operator Double() const;

    explicit operator const Number&() const;

    bool operator==(const Value& value) const;

    bool operator!=(const Value& value) const {
        return !operator==(value);
    }

    class BaseIterator {
    protected:
         BaseIterator(const Array::iterator& it) :
            m_type(Value::Type::ARRAY),
            m_array_iterator(it) { }

        BaseIterator(const Array::const_iterator& it) :
            m_type(Value::Type::ARRAY),
            m_array_const_iterator(it) { }

        BaseIterator(const Object::iterator& it) :
            m_type(Value::Type::OBJECT),
            m_object_iterator(it) { }

        BaseIterator(const Object::const_iterator& it) :
            m_type(Value::Type::OBJECT),
            m_object_const_iterator(it) { }

        void increment() {
            if (Value::Type::OBJECT == m_type) {
                m_object_iterator++;
            } else {
                m_array_iterator++;
            }
        }

        void decrement() {
            if (Value::Type::OBJECT == m_type) {
                m_object_iterator--;
            } else {
                m_array_iterator--;
            }
        }

        void const_increment() {
            if (Value::Type::OBJECT == m_type) {
                m_object_const_iterator++;
            } else {
                m_array_const_iterator++;
            }
        }

        void const_decrement() {
            if (Value::Type::OBJECT == m_type) {
                m_object_const_iterator--;
            } else {
                m_array_const_iterator--;
            }
        }

        Value& deref() {
            if (Value::Type::OBJECT == m_type) {
                return m_object_iterator->second;
            }
            return *m_array_iterator;
        }

        const Value& const_deref() const {
            if (Value::Type::OBJECT == m_type) {
                return m_object_const_iterator->second;
            }
            return *m_array_const_iterator;
        }

        bool is_equal(const BaseIterator& it) const {
            if (it.m_type != m_type) {
                return false;
            }

            if (Value::Type::OBJECT == m_type) {
                return (m_object_iterator == it.m_object_iterator);
            }
            return (m_array_iterator == it.m_array_iterator);
        }

        bool is_const_equal(const BaseIterator& it) const {
            if (it.m_type != m_type) {
                return false;
            }

            if (Value::Type::OBJECT == m_type) {
                return (m_object_const_iterator == it.m_object_const_iterator);
            }
            return (m_array_const_iterator == it.m_array_const_iterator);
        }
    public:
        const char* key() const;
    private:
        Value::Type m_type;

        union {
            Array::iterator m_array_iterator;
            Array::const_iterator m_array_const_iterator;
            Object::iterator m_object_iterator;
            Object::const_iterator m_object_const_iterator;
        };
    };

    class Iterator : public BaseIterator {
    public:
        Iterator() : BaseIterator(Array::iterator{}) { }

        Iterator(const Array::iterator& it) : BaseIterator(it) { }

        Iterator(const Object::iterator& it) : BaseIterator(it) { }

        Iterator& operator++() { increment(); return *this; }

        Iterator operator++(int) {
            Iterator temp(*this);
            increment();
            return temp;
        }

        Value& operator*() { return deref(); }

        Value& operator->() { return deref(); }

        bool operator!=(const Iterator& it) const { return !is_equal(it); }
    };

    class ConstIterator : public BaseIterator {
    public:
        ConstIterator() : BaseIterator(Array::const_iterator{}) { }

        ConstIterator(const Array::const_iterator& it) : BaseIterator(it) { }

        ConstIterator(const Object::const_iterator& it) : BaseIterator(it) { }

        const ConstIterator& operator++() { const_increment(); return *this; }

        const ConstIterator operator++(int) {
            ConstIterator temp(*this);
            const_increment();
            return temp;
        }

        const Value& operator*() const { return const_deref(); }

        const Value& operator->() const { return const_deref(); }

        bool operator!=(const ConstIterator& it) const {
            return !is_const_equal(it);
        }
    };

    Iterator begin();

    Iterator end();

    ConstIterator begin() const { return std::move(cbegin()); }

    ConstIterator end() const { return std::move(cend()); }

    ConstIterator cbegin() const;

    ConstIterator cend() const;

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
    void assert_container() const;
    void assert_type(Type type) const;
};

} /* namespace json */

#endif /* _JSON_HPP_ */
