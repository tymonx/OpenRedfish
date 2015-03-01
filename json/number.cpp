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
 * @file json/number.cpp
 *
 * @brief JSON number implementation
 * */

#include "number.hpp"

#include <cmath>
#include <limits>

using namespace json;

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

Number& Number::operator+=(const Number& number) {
    switch (m_type) {
    case Type::INT:
        m_int += Int(number);
        break;
    case Type::UINT:
        m_uint += Uint(number);
        break;
    case Type::DOUBLE:
        m_double += Double(number);
        break;
    default:
        break;
    }

    return *this;
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


